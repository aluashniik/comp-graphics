#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"

unsigned int static_object_ID_mapper[N_MAX_STATIC_OBJECTS];
unsigned int dynamic_object_ID_mapper[N_MAX_DYNAMIC_OBJECTS];
unsigned int camera_ID_mapper[N_MAX_CAMERAS];
unsigned int shader_ID_mapper[N_MAX_SHADERS];

void Axis_Object::define_axis() {
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Axis_Object::draw_axis(Shader_Simple* shader_simple, glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix) {
#define WC_AXIS_LENGTH		60.0f
	glm::mat4 ModelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix;

	glUseProgram(shader_simple->h_ShaderProgram);
	glUniformMatrix4fv(shader_simple->loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);


	glBindVertexArray(VAO);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(shader_simple->loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Scene::clock(int clock_id) { // currently one clock
	time_stamp = ++time_stamp % UINT_MAX;
}

void Scene::build_static_world() {
	static_geometry_data.building.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.building);

	static_geometry_data.light.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.light);

	static_geometry_data.dragon.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.dragon);

	static_geometry_data.optimus.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.optimus);

	static_geometry_data.cat.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.cat);

	static_geometry_data.godzilla.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.godzilla);

	static_geometry_data.helicopter.define_object();
	static_object_ID_mapper[STATIC_OBJECT_BUILDING] = static_objects.size();
	static_objects.push_back(static_geometry_data.helicopter);

}

void Scene::build_dynamic_world() {
	dynamic_geometry_data.ben_d.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_TIGER] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.ben_d);

	dynamic_geometry_data.spider_d.define_object();
	dynamic_object_ID_mapper[DYNAMIC_OBJECT_TIGER] = dynamic_objects.size();
	dynamic_objects.push_back(dynamic_geometry_data.spider_d);

}


void Scene::create_camera_list(int win_width, int win_height, float win_aspect_ratio) {
	camera_list.clear();

	camera_data.cam_main.define_camera(win_width, win_height, win_aspect_ratio);
	camera_ID_mapper[CAMERA_MAIN] = camera_list.size();
	camera_list.push_back(camera_data.cam_main);

	// 메인 카메라 뷰포트 정보 가져오기
	int main_cam_viewport_x = camera_data.cam_main.view_port.x;
	int main_cam_viewport_y = camera_data.cam_main.view_port.y;
	int main_cam_viewport_w = camera_data.cam_main.view_port.w;
	int main_cam_viewport_h = camera_data.cam_main.view_port.h;

	//정적 CCTV 카메라 3개 배치
	int cctv_static_width = main_cam_viewport_w / 4.5; // 메인 뷰포트 너비의 1/4.5
	int cctv_static_height = cctv_static_width * 9 / 16; // 16:9 비율
	int padding = 10;
	int cctv_vertical_offset = 50;

	// CCTV 0 
	camera_data.cam_cctv_0.define_camera(win_width, win_height, (float)cctv_static_width / cctv_static_height);
	camera_data.cam_cctv_0.view_port.x = main_cam_viewport_x + main_cam_viewport_w - cctv_static_width - padding;
	camera_data.cam_cctv_0.view_port.y = main_cam_viewport_y + padding + cctv_vertical_offset;
	camera_data.cam_cctv_0.view_port.w = cctv_static_width;
	camera_data.cam_cctv_0.view_port.h = cctv_static_height;
	camera_ID_mapper[CAMERA_CCTV_0] = camera_list.size();
	camera_list.push_back(std::ref(camera_data.cam_cctv_0));

	// CCTV 1 
	camera_data.cam_cctv_1.define_camera(win_width, win_height, (float)cctv_static_width / cctv_static_height);
	camera_data.cam_cctv_1.view_port.x = main_cam_viewport_x + main_cam_viewport_w - cctv_static_width - padding;
	camera_data.cam_cctv_1.view_port.y = main_cam_viewport_y + padding + cctv_static_height + padding + cctv_vertical_offset;
	camera_data.cam_cctv_1.view_port.w = cctv_static_width;
	camera_data.cam_cctv_1.view_port.h = cctv_static_height;
	camera_ID_mapper[CAMERA_CCTV_1] = camera_list.size();
	camera_list.push_back(std::ref(camera_data.cam_cctv_1));

	// CCTV 2 
	camera_data.cam_cctv_2.define_camera(win_width, win_height, (float)cctv_static_width / cctv_static_height);
	camera_data.cam_cctv_2.view_port.x = main_cam_viewport_x + main_cam_viewport_w - cctv_static_width - padding;
	camera_data.cam_cctv_2.view_port.y = main_cam_viewport_y + padding + (cctv_static_height + padding) * 2 + cctv_vertical_offset;
	camera_data.cam_cctv_2.view_port.w = cctv_static_width;
	camera_data.cam_cctv_2.view_port.h = cctv_static_height;
	camera_ID_mapper[CAMERA_CCTV_2] = camera_list.size();
	camera_list.push_back(std::ref(camera_data.cam_cctv_2));

	// 동적 CCTV 카메라 배치 (메인 뷰포트 좌측 상단) 
	int cctv_dynamic_width = main_cam_viewport_w / 4; 
	int cctv_dynamic_height = cctv_dynamic_width * 9 / 16;
	camera_data.cam_cctv_dynamic.define_camera(win_width, win_height, (float)cctv_dynamic_width / cctv_dynamic_height);
	camera_data.cam_cctv_dynamic.view_port.x = main_cam_viewport_x + padding; 
	camera_data.cam_cctv_dynamic.view_port.y = main_cam_viewport_y + main_cam_viewport_h - cctv_dynamic_height - padding; 
	camera_data.cam_cctv_dynamic.view_port.w = cctv_dynamic_width;
	camera_data.cam_cctv_dynamic.view_port.h = cctv_dynamic_height;
	camera_ID_mapper[CAMERA_CCTV_DYNAMIC] = camera_list.size();
	camera_list.push_back(std::ref(camera_data.cam_cctv_dynamic));


	float ortho_aspect = (float)cctv_static_width / cctv_static_height; // CCTV와 동일한 종횡비 사용

	// 정면도 카메라 (CAMERA_FRONT_ORTHO)
	camera_data.cam_front_ortho.camera_id = CAMERA_FRONT_ORTHO; // ID 설정
	camera_data.cam_front_ortho.define_camera(win_width, win_height, ortho_aspect);
	camera_data.cam_front_ortho.view_port.x = main_cam_viewport_x + main_cam_viewport_w - cctv_static_width - padding;
	camera_data.cam_front_ortho.view_port.y = main_cam_viewport_y + padding + cctv_vertical_offset;
	camera_data.cam_front_ortho.view_port.w = cctv_static_width;
	camera_data.cam_front_ortho.view_port.h = cctv_static_height;
	camera_ID_mapper[CAMERA_FRONT_ORTHO] = camera_list.size();
	camera_list.push_back(std::ref(camera_data.cam_front_ortho));
	camera_data.cam_front_ortho.flag_valid = false; // Initially inactive

	// 측면도 카메라 (CAMERA_SIDE_ORTHO)
	camera_data.cam_side_ortho.camera_id = CAMERA_SIDE_ORTHO; // ID 설정
	camera_data.cam_side_ortho.define_camera(win_width, win_height, ortho_aspect);
	camera_data.cam_side_ortho.view_port.x = main_cam_viewport_x + main_cam_viewport_w - cctv_static_width - padding;
	camera_data.cam_side_ortho.view_port.y = main_cam_viewport_y + padding + cctv_static_height + padding + cctv_vertical_offset;
	camera_data.cam_side_ortho.view_port.w = cctv_static_width;
	camera_data.cam_side_ortho.view_port.h = cctv_static_height;
	camera_ID_mapper[CAMERA_SIDE_ORTHO] = camera_list.size();
	camera_list.push_back(std::ref(camera_data.cam_side_ortho));
	camera_data.cam_side_ortho.flag_valid = false; // Initially inactive

	// 상면도 카메라 (CAMERA_TOP_ORTHO)
	camera_data.cam_top_ortho.camera_id = CAMERA_TOP_ORTHO; // ID 설정
	camera_data.cam_top_ortho.define_camera(win_width, win_height, ortho_aspect);
	camera_data.cam_top_ortho.view_port.x = main_cam_viewport_x + main_cam_viewport_w - cctv_static_width - padding;
	camera_data.cam_top_ortho.view_port.y = main_cam_viewport_y + padding + (cctv_static_height + padding) * 2 + cctv_vertical_offset;
	camera_data.cam_top_ortho.view_port.w = cctv_static_width;
	camera_data.cam_top_ortho.view_port.h = cctv_static_height;
	camera_ID_mapper[CAMERA_TOP_ORTHO] = camera_list.size();
	camera_list.push_back(std::ref(camera_data.cam_top_ortho));
	camera_data.cam_top_ortho.flag_valid = false; // Initially inactive
}

void Scene::build_shader_list() {
	shader_data.shader_simple.prepare_shader();
	shader_ID_mapper[SHADER_SIMPLE] = shader_list.size();
	shader_list.push_back(shader_data.shader_simple);
}

void Scene::initialize() {
	axis_object.define_axis();
	build_static_world();
	build_dynamic_world();
	create_camera_list(window.width, window.height, window.aspect_ratio);
	build_shader_list();
}

void Scene::draw_static_world() {
	glm::mat4 ModelViewProjectionMatrix;
	for (auto static_object = static_objects.begin(); static_object != static_objects.end(); static_object++) {
		if (static_object->get().flag_valid == false) continue;
		static_object->get().draw_object(ViewMatrix, ProjectionMatrix, shader_kind, shader_list);
	}
}

void Scene::draw_dynamic_world() {
	glm::mat4 ModelViewProjectionMatrix;
	for (auto dynamic_object = dynamic_objects.begin(); dynamic_object != dynamic_objects.end(); dynamic_object++) {
		if (dynamic_object->get().flag_valid == false) continue;
		dynamic_object->get().draw_object(ViewMatrix, ProjectionMatrix, shader_kind, shader_list, time_stamp);
	}
}

void Scene::draw_axis() {
	axis_object.draw_axis(static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[SHADER_SIMPLE]].get()),
		ViewMatrix, ProjectionMatrix);
}

void Scene::draw_world() {
	draw_axis();
	draw_static_world();
	draw_dynamic_world();
}