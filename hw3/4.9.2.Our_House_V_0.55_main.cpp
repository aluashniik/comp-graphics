#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shaders/LoadShaders.h"
#include "Scene_Definitions.h"
#include "Camera.h"
#include <vector> //

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 

Scene scene;

int last_mouse_x = -1;
int last_mouse_y = -1;
bool is_mouse_rotating = false;

bool is_dynamic_cctv_moving_y = false;
bool is_shift_pressed = false;

bool show_ortho_views = false;

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto camera_wrapper : scene.camera_list) {
		Camera& camera = camera_wrapper.get();

		if (camera.flag_valid == false) continue;

		glViewport(camera.view_port.x, camera.view_port.y,
			camera.view_port.w, camera.view_port.h);

		scene.ViewMatrix = camera.ViewMatrix;
		scene.ProjectionMatrix = camera.ProjectionMatrix;

		scene.draw_world();

		if (camera.camera_id >= CAMERA_CCTV_0 && camera.camera_id <= CAMERA_TOP_ORTHO) {
			glm::mat4 ortho = glm::ortho(0.0f, (float)camera.view_port.w, 0.0f, (float)camera.view_port.h);
			glUseProgram(scene.shader_program_border);
			glUniformMatrix4fv(glGetUniformLocation(scene.shader_program_border, "uProjection"), 1, GL_FALSE, glm::value_ptr(ortho));
			glBindVertexArray(scene.vao_border);
			glDrawArrays(GL_LINE_LOOP, 0, 4);
			glBindVertexArray(0);
			glUseProgram(0);
		}

		if (camera.flag_draw_axes) {
			draw_axes_core_profile(camera);
		}
	}
	glutSwapBuffers();
}

#define CAMERA_MOVE_SPEED 10.0f // 적절한 값으로 정의되어 있다고 가정
#define CAMERA_ROTATION_SPEED 1.0f // 적절한 값으로 정의되어 있다고 가정
#define CAMERA_ZOOM_SPEED 2.0f // 적절한 값으로 정의되어 있다고 가정

void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;
	bool camera_updated = false;

	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop();
		break;

	case 'c': // Cull face toggle
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			fprintf(stdout, "^^^ No faces are culled.\n");
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			fprintf(stdout, "^^^ Back faces are culled.\n");
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			fprintf(stdout, "^^^ Front faces are culled.\n");
			break;
		}
		camera_updated = true; // Scene state changed, so redraw
		break;

	case 'f': // Polygon fill toggle
		polygon_fill_on = 1 - polygon_fill_on;
		if (polygon_fill_on) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fprintf(stdout, "^^^ Polygon filling enabled.\n");
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fprintf(stdout, "^^^ Line drawing enabled.\n");
		}
		camera_updated = true;
		break;

	case 'd': // Depth test toggle
		depth_test_on = 1 - depth_test_on;
		if (depth_test_on) {
			glEnable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test enabled.\n");
		}
		else {
			glDisable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test disabled.\n");
		}
		camera_updated = true;
		break;

	case 't': // 주 카메라와 CCTV의 카메라 프레임 토글 
	{ 
		for (auto& camera_wrapper : scene.camera_list) {
			Camera& cam = camera_wrapper.get();

			if (cam.camera_id == CAMERA_MAIN ||
				(cam.camera_id >= CAMERA_CCTV_0 && cam.camera_id <= CAMERA_CCTV_DYNAMIC)) {
				cam.flag_draw_axes = !cam.flag_draw_axes;
			}
		}
		fprintf(stdout, "^^^ Camera axes display toggled.\n");
		camera_updated = true;
	} 
	break;

	////////////////////////////////////////////////////////////
	// 메인 카메라 이동 
	case 'w': //move forward
	case 's': //move backward 
	case 'a': //strafe left 
	case 'D': //strafe right 
	case 'e': //move up 
	case 'q': //move down 
	{
		Camera& main_cam = scene.camera_data.cam_main; // 메인 카메라 직접 참조
		if (main_cam.flag_move) {
			glm::vec3 move_direction(0.0f);
			switch (key) {
			case 'w': // Forward
				move_direction = -main_cam.cam_view.naxis;
				fprintf(stdout, "^^^ Main Camera: Forward\n");
				break;
			case 's': // Backward
				move_direction = main_cam.cam_view.naxis;
				fprintf(stdout, "^^^ Main Camera: Backward\n");
				break;
			case 'a': // Left
				move_direction = -main_cam.cam_view.uaxis;
				fprintf(stdout, "^^^ Main Camera: Left\n");
				break;
			case 'D': // Right
				move_direction = main_cam.cam_view.uaxis;
				fprintf(stdout, "^^^ Main Camera: Right\n");
				break;
			case 'e': // Up 
				move_direction = main_cam.cam_view.vaxis;
				fprintf(stdout, "^^^ Main Camera: Up\n");
				break;
			case 'q': // Down
				move_direction = -main_cam.cam_view.vaxis;
				fprintf(stdout, "^^^ Main Camera: Down\n");
				break;
			}
			main_cam.cam_view.pos += move_direction * CAMERA_MOVE_SPEED;

			main_cam.ViewMatrix = glm::lookAt(
				main_cam.cam_view.pos,
				main_cam.cam_view.pos - main_cam.cam_view.naxis,
				main_cam.cam_view.vaxis
			);
			main_cam.update_camera_axes(); 
			camera_updated = true;
		}
	}
	break;

	// 메인 카메라 회전 
	case 'j': // Yaw left
	case 'l': // Yaw right
	case 'i': // Pitch up
	case 'k': // Pitch down
	case 'u': // Roll left 
	case 'o': // Roll right 
	{
		Camera& main_cam = scene.camera_data.cam_main;
		if (main_cam.flag_move) {
			switch (key) {
			case 'j': // Yaw left
				main_cam.rotate_around_v(CAMERA_ROTATION_SPEED * 5.0f);
				fprintf(stdout, "^^^ Main Camera: Yaw Left\n");
				break;
			case 'l': // Yaw right
				main_cam.rotate_around_v(-CAMERA_ROTATION_SPEED * 5.0f);
				fprintf(stdout, "^^^ Main Camera: Yaw Right\n");
				break;
			case 'i': // Pitch up
				main_cam.rotate_around_u(CAMERA_ROTATION_SPEED * 5.0f);
				fprintf(stdout, "^^^ Main Camera: Pitch Up\n");
				break;
			case 'k': // Pitch down
				main_cam.rotate_around_u(-CAMERA_ROTATION_SPEED * 5.0f);
				fprintf(stdout, "^^^ Main Camera: Pitch Down\n");
				break;
			case 'u': // Roll left (around N axis)
				main_cam.rotate_around_n(CAMERA_ROTATION_SPEED * 5.0f);
				fprintf(stdout, "^^^ Main Camera: Roll Left\n");
				break;
			case 'o': // Roll right (around N axis)
				main_cam.rotate_around_n(-CAMERA_ROTATION_SPEED * 5.0f);
				fprintf(stdout, "^^^ Main Camera: Roll Right\n");
				break;
			}
			camera_updated = true;
		}
	}
	break;
	case 'Z': // ortho camera로 변환
		show_ortho_views = !show_ortho_views;
		if (show_ortho_views) {
			fprintf(stdout, "^^^ Displaying Ortho Views\n");
			// 정적 카메라에서 ortho로 
			scene.camera_data.cam_cctv_0.flag_valid = false;
			scene.camera_data.cam_cctv_1.flag_valid = false;
			scene.camera_data.cam_cctv_2.flag_valid = false;

			scene.camera_data.cam_front_ortho.flag_valid = true;
			scene.camera_data.cam_side_ortho.flag_valid = true;
			scene.camera_data.cam_top_ortho.flag_valid = true;
		}
		else {
			fprintf(stdout, "^^^ Displaying Static CCTV Views.\n");
			 scene.camera_data.cam_cctv_0.flag_valid = true;
			 scene.camera_data.cam_cctv_1.flag_valid = true;
			 scene.camera_data.cam_cctv_2.flag_valid = true;

			scene.camera_data.cam_front_ortho.flag_valid = false;
			scene.camera_data.cam_side_ortho.flag_valid = false;
			scene.camera_data.cam_top_ortho.flag_valid = false;
		}
		camera_updated = true;
		break;

	// 메인 카메라 줌 
	case '+': // zoom in 
	case '-': // zoom uut
	{
		Perspective_Camera* p_main_cam = dynamic_cast<Perspective_Camera*>(&scene.camera_data.cam_main);
		if (p_main_cam && p_main_cam->flag_move) {
			if (key == '+') {
				p_main_cam->zoom(-CAMERA_ZOOM_SPEED);
				fprintf(stdout, "^^^ Main Camera: Zoom In\n");
			}
			else {
				p_main_cam->zoom(CAMERA_ZOOM_SPEED); 
				fprintf(stdout, "^^^ Main Camera: Zoom Out\n");
			}
			camera_updated = true;
		}
	}
	break;

	// 동적 CCTV 카메라
	case 'P': // 동적 CCTV 줌 인 
	{ 
		Perspective_Camera* p_dynamic_cctv_zoom_in = dynamic_cast<Perspective_Camera*>(&scene.camera_data.cam_cctv_dynamic);
		if (p_dynamic_cctv_zoom_in && p_dynamic_cctv_zoom_in->flag_move) {
			p_dynamic_cctv_zoom_in->zoom(-CAMERA_ZOOM_SPEED);
			fprintf(stdout, "^^^ Dynamic CCTV: Zoom In\n");
			camera_updated = true;
		}
	} 
	break;

	case 'O': // 동적 CCTV 줌 아웃 
	{ 
		Perspective_Camera* p_dynamic_cctv_zoom_out = dynamic_cast<Perspective_Camera*>(&scene.camera_data.cam_cctv_dynamic);
		if (p_dynamic_cctv_zoom_out && p_dynamic_cctv_zoom_out->flag_move) {
			p_dynamic_cctv_zoom_out->zoom(CAMERA_ZOOM_SPEED);
			fprintf(stdout, "^^^ Dynamic CCTV: Zoom Out\n");
			camera_updated = true;
		}
	} 
	break;
	} 
	if (camera_updated) {
		glutPostRedisplay();
	}
}

//to detect shift key press/release
void special_keyboard(int key, int x, int y) {
	if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R) {
		is_shift_pressed = true;
		fprintf(stdout, "^^^ Shift key pressed.\n");
	}
}

void special_keyboard_up(int key, int x, int y) {
	if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R) {
		is_shift_pressed = false;
		fprintf(stdout, "^^^ Shift key released.\n");
	}
}

void mouse_button(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			if (is_shift_pressed) {
				is_dynamic_cctv_moving_y = true; // activate Y-movement mode for dynamic CCTV
				fprintf(stdout, "^^^ Mouse: Dynamic CCTV Y-movement started\n");
			}
			else {
				is_mouse_rotating = true; // normal rotation mode for main camera
				fprintf(stdout, "^^^ Mouse: Rotation started\n");
			}
			last_mouse_x = x;
			last_mouse_y = y;
		}
		else { // GLUT_UP
			is_mouse_rotating = false;
			is_dynamic_cctv_moving_y = false; // deactivate Y-movement mode
			fprintf(stdout, "^^^ Mouse: Interaction stopped\n");
		}
	}
}


void mouse_motion(int x, int y) {
	float delta_x = (float)x - last_mouse_x;
	float delta_y = (float)y - last_mouse_y;

	last_mouse_x = x;
	last_mouse_y = y;

	if (is_dynamic_cctv_moving_y) {
		Perspective_Camera& dynamic_cctv = scene.camera_data.cam_cctv_dynamic;
		if (dynamic_cctv.flag_move) {
			//get the current camera position from its ViewMatrix
			glm::vec3 current_pos = glm::vec3(glm::inverse(dynamic_cctv.ViewMatrix)[3]);

			//move along the world's Y-axis
			current_pos.y += delta_y * (CAMERA_MOVE_SPEED / 5.0f); 
			glm::mat4 inv_view = glm::inverse(dynamic_cctv.ViewMatrix);
			glm::vec3 cam_pos = glm::vec3(inv_view[3]);
			glm::vec3 cam_forward = -glm::vec3(inv_view[2]); //-n axis
			glm::vec3 cam_up = glm::vec3(inv_view[1]); //v axis

			glm::vec3 new_pos = current_pos;
			static glm::vec3 initial_cctv_look_at_offset = glm::vec3(0.0f); 
			glm::vec3 current_look_at = cam_pos + cam_forward * 100.0f; //look 100 units forward
			current_look_at.y += delta_y * (CAMERA_MOVE_SPEED / 5.0f); //match Y movement for look-at point

			dynamic_cctv.ViewMatrix = glm::lookAt(
				current_pos,          //new eye position
				current_look_at,      //new Center
				cam_up                //maintain existing up vector
			);

			dynamic_cctv.update_camera_axes();
			glutPostRedisplay();
		}
	}
	else if (is_mouse_rotating) {
		for (auto& camera_wrapper : scene.camera_list) {
			Camera& cam = camera_wrapper.get();
			if (cam.camera_id == CAMERA_MAIN && cam.flag_move) {
				cam.rotate_around_v(-delta_x * CAMERA_ROTATION_SPEED);
				cam.rotate_around_u(-delta_y * CAMERA_ROTATION_SPEED);
				glutPostRedisplay();
				break;
			}
		}
	}
}

void mouse_wheel(int button, int dir, int x, int y) {
	if (dir > 0) { //zoom in
		for (auto& camera_wrapper : scene.camera_list) {
			Perspective_Camera* p_cam = dynamic_cast<Perspective_Camera*>(&camera_wrapper.get());
			if (p_cam && p_cam->camera_id == CAMERA_MAIN && p_cam->flag_move) {
				p_cam->zoom(-CAMERA_ZOOM_SPEED * 2.0f); // decrease FOV for zoom in
				fprintf(stdout, "^^^ Mouse Wheel: Zoom In\n");
				glutPostRedisplay();
				break;
			}
		}
	}
	else if (dir < 0) { //zoom out
		for (auto& camera_wrapper : scene.camera_list) {
			Perspective_Camera* p_cam = dynamic_cast<Perspective_Camera*>(&camera_wrapper.get());
			if (p_cam && p_cam->camera_id == CAMERA_MAIN && p_cam->flag_move) {
				p_cam->zoom(CAMERA_ZOOM_SPEED * 2.0f); 
				fprintf(stdout, "^^^ Mouse Wheel: Zoom Out\n");
				glutPostRedisplay();
				break;
			}
		}
	}
}

void reshape(int width, int height) {
	scene.window.width = width;
	scene.window.height = height;
	scene.window.aspect_ratio = (float)width / height;
	// Scene::create_camera_list 함수 호출 (scene 객체에 대한 멤버 함수)
	scene.create_camera_list(scene.window.width, scene.window.height, scene.window.aspect_ratio);
	glutPostRedisplay();
}


void timer_scene(int index) {
	scene.clock(0);
	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, 0);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_keyboard); // 
	glutSpecialUpFunc(special_keyboard_up);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutMouseFunc(mouse_button);
	glutMotionFunc(mouse_motion);
	glutMouseWheelFunc(mouse_wheel); 
}

void draw_axes_core_profile(Camera& camera) {
	if (!camera.flag_draw_axes) return;

	glUseProgram(scene.shader_program_axes);
	glm::mat4 model = glm::inverse(camera.ViewMatrix);
	glUniformMatrix4fv(glGetUniformLocation(scene.shader_program_axes, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(scene.shader_program_axes, "uView"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glUniformMatrix4fv(glGetUniformLocation(scene.shader_program_axes, "uProjection"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	glBindVertexArray(scene.vao_axes);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Scene::initialize_axes_geometry() {
	float axis_data[] = {
		0, 0, 0,  1, 0, 0,   50, 0, 0,  1, 0, 0,
		0, 0, 0,  0, 1, 0,   0, 50, 0,  0, 1, 0,
		0, 0, 0,  0, 0, 1,   0, 0, 50,  0, 0, 1
	};
	glGenVertexArrays(1, &vao_axes);
	glGenBuffers(1, &vbo_axes);
	glBindVertexArray(vao_axes);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axis_data), axis_data, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Scene::initialize_border_geometry() {
	float border[] = {
		0, 0,
		1, 0,
		1, 1,
		0, 1
	};
	glGenVertexArrays(1, &vao_border);
	glGenBuffers(1, &vbo_border);
	glBindVertexArray(vao_border);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_border);
	glBufferData(GL_ARRAY_BUFFER, sizeof(border), border, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);
	scene.initialize_axes_geometry();
	scene.initialize_border_geometry();
}

void initialize_renderer(void) {
	register_callbacks();
	initialize_OpenGL();
	scene.initialize();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void print_message(const char* m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char* program_name, char messages[][256], int n_message_lines) {


	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170/AIE4012 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char* argv[]) {
	char program_name[256] = "Sogang CSE4170/AIE4120 Our_House_GLSL_V_0.55";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: w/s/a/D/q/e for movement, J/L/I/K/U/O for rotation,T for toggle, O/P for zooming dynamic camera, +/- for zoom. Shift + Left mouse button for Dynamic CCTV Y-axis movement. Z for Ortho views"
	};

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}