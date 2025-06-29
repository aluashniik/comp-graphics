/////////////////////////////////////////////////////////

#define _CRT_SECURE_NO_WARNINGS

#include "Camera.h"
#define TO_RADIAN 0.01745329252f
#define TO_DEGREE 57.295779513f


void Perspective_Camera::define_camera(int win_width, int win_height, float win_aspect_ratio) {
	// glm::mat3 R33_t; 
	// glm::mat4 T;     

	switch (camera_id) {
	case CAMERA_MAIN:
		flag_valid = true;
		flag_move = true; // 메인 카메라는 움직일 수 있음

		ViewMatrix = glm::lookAt(glm::vec3(-600.0f, -600.0f, 400.0f), glm::vec3(125.0f, 80.0f, 25.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); // 메인 카메라 초기 포즈
		update_camera_axes(); // ViewMatrix 설정 후 축 업데이트

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 15.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;

		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		//view_port.x = 150; view_port.y = 100; view_port.w = win_width-300; view_port.h = win_height-200;
		view_port.x = 10; view_port.y = 10; view_port.w = win_width - 20; view_port.h = win_height - 20;

		break;

		//정적 CCTV 카메라 
	case CAMERA_CCTV_0: // 방의 중간 쪽에 배치되어 있으며 ben을 옆에서 보는 중
		flag_valid = true;
		flag_move = false; // 정적 CCTV는 고정
		ViewMatrix = glm::lookAt(glm::vec3(150.0f, 100.0f, 40.0f), glm::vec3(125.0f, 170.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
		update_camera_axes();

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 90.0f * TO_RADIAN; 
		cam_proj.params.pers.aspect = win_aspect_ratio; 
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;
		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect, cam_proj.params.pers.n, cam_proj.params.pers.f);
		break;

	case CAMERA_CCTV_1: // spider의 위치에서 조금 떨어져서 위에서 배치되었음
		flag_valid = true;
		flag_move = false;
		ViewMatrix = glm::lookAt(glm::vec3(177.0f, 53.0f, 48.0f), glm::vec3(220.0f, 70.0f, 5.0f),
			glm::vec3(0.0f, 0.0f, 1.0f)); 
		update_camera_axes();

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 72.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;
		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		break;

	case CAMERA_CCTV_2: // 입구에 x쪽에서 중간에서 배치되어 있으므로 y의 끝쪽으로 바라보고 있음
		flag_valid = true;
		flag_move = false;

		ViewMatrix = glm::lookAt(glm::vec3(20.0f, 90.0f, 38.0f),
			glm::vec3(180.0f, 90.0f, 30.0f),
			glm::vec3(0.0f, 0.0f, 1.0f));
		update_camera_axes();

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 90.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;
		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		break;

		// 동적 CCTV 카메라 
	case CAMERA_CCTV_DYNAMIC: //ben 물체 앞에서 배치되어 있고, shift+left mouse button 누르면 y-axis으로 spider의 움직임을 관찰할 수 있음
		flag_valid = true;
		flag_move = true;
		//	ViewMatrix = glm::lookAt(glm::vec3(240.0f, 146.0f, 45.0f), glm::vec3(0.0f, 70.0f, 20.0f),glm::vec3(0.0f, 0.0f, 1.0f)); // 

		ViewMatrix = glm::lookAt(glm::vec3(220.0f, 144.0f, 28.0f), glm::vec3(0.0f, 146.0f, -20.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // 
		update_camera_axes();

		cam_proj.projection_type = CAMERA_PROJECTION_PERSPECTIVE;
		cam_proj.params.pers.fovy = 65.0f * TO_RADIAN;
		cam_proj.params.pers.aspect = win_aspect_ratio;
		cam_proj.params.pers.n = 1.0f;
		cam_proj.params.pers.f = 50000.0f;
		ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect,
			cam_proj.params.pers.n, cam_proj.params.pers.f);
		break;
	}
}


void Orthographic_Camera::define_camera(int win_width, int win_height, float win_aspect_ratio) {
		flag_valid = true;
		flag_move = false; 
		cam_proj.projection_type = CAMERA_PROJECTION_ORTHOGRAPHIC;


		float view_width = 300.0f;  
		float view_height = view_width / win_aspect_ratio; 
		
		float z_near = -500.0f;
		float z_far = 500.0f;

		switch (camera_id) {
		case CAMERA_FRONT_ORTHO: 
			ViewMatrix = glm::lookAt(glm::vec3(-view_width / 2.0f, 80.0f, 50.0f), 
				glm::vec3(0.0f, 80.0f, 50.0f),  
				glm::vec3(0.0f, 0.0f, 1.0f)); 

			cam_proj.params.ortho.left = -view_width / 2.0f;
			cam_proj.params.ortho.right = view_width / 2.0f;
			cam_proj.params.ortho.bottom = -view_height / 2.0f;
			cam_proj.params.ortho.top = view_height / 2.0f;
			cam_proj.params.ortho.n = z_near;
			cam_proj.params.ortho.f = z_far;
			break;

		case CAMERA_SIDE_ORTHO:
			ViewMatrix = glm::lookAt(glm::vec3(115.0f, -view_width / 2.0f, 50.0f), 
				glm::vec3(115.0f, 0.0f, 50.0f),      
				glm::vec3(0.0f, 0.0f, 1.0f));      
			cam_proj.params.ortho.left = -view_width / 2.0f;
			cam_proj.params.ortho.right = view_width / 2.0f;
			cam_proj.params.ortho.bottom = -view_height / 2.0f;
			cam_proj.params.ortho.top = view_height / 2.0f;
			cam_proj.params.ortho.n = z_near;
			cam_proj.params.ortho.f = z_far;
			break;

		case CAMERA_TOP_ORTHO: 
			ViewMatrix = glm::lookAt(glm::vec3(115.0f, 80.0f, view_width / 2.0f),   
				glm::vec3(115.0f, 80.0f, 0.0f),  
				glm::vec3(0.0f, 1.0f, 0.0f));     

			cam_proj.params.ortho.left = -view_width / 2.0f;
			cam_proj.params.ortho.right = view_width / 2.0f;
			cam_proj.params.ortho.bottom = -view_height / 2.0f;
			cam_proj.params.ortho.top = view_height / 2.0f;
			cam_proj.params.ortho.n = z_near;
			cam_proj.params.ortho.f = z_far;
			break;
		}
		ProjectionMatrix = glm::ortho(cam_proj.params.ortho.left, cam_proj.params.ortho.right,
			cam_proj.params.ortho.bottom, cam_proj.params.ortho.top,
			cam_proj.params.ortho.n, cam_proj.params.ortho.f);
		update_camera_axes();
}

void Orthographic_Camera::zoom_ortho(float delta_size) {
	float current_width = cam_proj.params.ortho.right - cam_proj.params.ortho.left;
	float current_height = cam_proj.params.ortho.top - cam_proj.params.ortho.bottom;

	float new_width = current_width + delta_size;
	if (new_width < 1.0f) new_width = 1.0f; 

	float aspect = cam_proj.params.ortho.right / cam_proj.params.ortho.top; 
	float new_height = new_width / aspect;

	cam_proj.params.ortho.left = -new_width / 2.0f;
	cam_proj.params.ortho.right = new_width / 2.0f;
	cam_proj.params.ortho.bottom = -new_height / 2.0f;
	cam_proj.params.ortho.top = new_height / 2.0f;

	ProjectionMatrix = glm::ortho(cam_proj.params.ortho.left, cam_proj.params.ortho.right,
		cam_proj.params.ortho.bottom, cam_proj.params.ortho.top,
		cam_proj.params.ortho.n, cam_proj.params.ortho.f);
}

void Camera::rotate_around_u(float angle_degrees) {
	ViewMatrix = glm::rotate(ViewMatrix, glm::radians(angle_degrees), cam_view.uaxis);
	update_camera_axes();
}

void Camera::rotate_around_v(float angle_degrees) {
	ViewMatrix = glm::rotate(ViewMatrix, glm::radians(angle_degrees), cam_view.vaxis);
	update_camera_axes();
}

void Camera::rotate_around_n(float angle_degrees) {
	ViewMatrix = glm::rotate(ViewMatrix, glm::radians(angle_degrees), cam_view.naxis);
	update_camera_axes();
}

void Camera::update_camera_axes() {
	cam_view.uaxis = glm::normalize(glm::vec3(ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]));
	cam_view.vaxis = glm::normalize(glm::vec3(ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]));
	cam_view.naxis = glm::normalize(glm::vec3(ViewMatrix[0][2], ViewMatrix[1][2], ViewMatrix[2][2]));

	glm::mat4 inverted_view_matrix = glm::inverse(ViewMatrix);
	cam_view.pos = glm::vec3(inverted_view_matrix[3]);
}

void Perspective_Camera::zoom(float delta_fovy) { // 메인 카메라 fov 조절
	float current_fovy_degrees = cam_proj.params.pers.fovy * TO_DEGREE;
	current_fovy_degrees += delta_fovy;

	if (current_fovy_degrees < 1.0f) current_fovy_degrees = 1.0f;
	if (current_fovy_degrees > 170.0f) current_fovy_degrees = 170.0f;

	cam_proj.params.pers.fovy = current_fovy_degrees * TO_RADIAN;
	ProjectionMatrix = glm::perspective(cam_proj.params.pers.fovy, cam_proj.params.pers.aspect, cam_proj.params.pers.n, cam_proj.params.pers.f);
}