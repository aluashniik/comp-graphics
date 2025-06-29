
#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

enum Camera_Projection_TYPE {
	CAMERA_PROJECTION_PERSPECTIVE = 0, CAMERA_PROJECTION_ORTHOGRAPHIC=1
};

enum Camera_ID {
	CAMERA_MAIN = 0,
	CAMERA_CCTV_0, // 정적 CCTV 카메라 1
	CAMERA_CCTV_1, // 정적 CCTV 카메라 2
	CAMERA_CCTV_2, // 정적 CCTV 카메라 3
	CAMERA_CCTV_DYNAMIC, // 동적 CCTV 카메라

	CAMERA_FRONT_ORTHO, // 직교투영 정면도
	CAMERA_SIDE_ORTHO,  // 직교투영 측면도
	CAMERA_TOP_ORTHO,   // 직교투영 상면도

	//CAMERA_SIDE_FRONT, CAMERA_TOP, CAMERA_SIDE, CAMERA_CC_0, CAMERA_CC_1
};

struct Camera_View {
	glm::vec3 pos;
	glm::vec3 uaxis, vaxis, naxis;
};

struct Camera_Projection {
	Camera_Projection_TYPE projection_type;
	union {
		struct {
			float fovy, aspect, n, f;
		} pers;
		struct{
			float left, right;  
			float bottom, top;  
			float n, f;         
		} ortho;
	} params;
};

struct View_Port {
	float x, y, w, h;
};

struct Camera {
	Camera_ID camera_id;
	Camera_View cam_view;
	glm::mat4 ViewMatrix;
	Camera_Projection cam_proj;
	glm::mat4 ProjectionMatrix;
	View_Port view_port;
	bool flag_draw_axes; // 카메라 프레임 토글을 위한 플래그

	Camera(Camera_ID _camera_id) : camera_id(_camera_id), flag_draw_axes(false) {} // 초기값 false
	virtual ~Camera() = default;

	bool flag_valid;
	bool flag_move; // 카메라 이동 허용 여부
	bool flag_drag_axes;

	void rotate_around_u(float angle_degrees);
	void rotate_around_v(float angle_degrees);
	void rotate_around_n(float angle_degrees);
	void update_camera_axes();

	virtual void define_camera(int win_width, int win_height, float win_aspect_ratio) = 0;
};

struct Perspective_Camera : public Camera {
	Perspective_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}
	void define_camera(int win_width, int win_height, float win_aspect_ratio);
	void zoom(float delta_fovy); 
};

struct Orthographic_Camera : public Camera {
	Orthographic_Camera(Camera_ID _camera_id) : Camera(_camera_id) {}
	void define_camera(int win_width, int win_height, float win_aspect_ratio);
	
	void zoom_ortho(float delta_size);
};

struct Camera_Data {
	Perspective_Camera cam_main{ CAMERA_MAIN };
	/////////////
	Perspective_Camera cam_cctv_0{ CAMERA_CCTV_0 };
	Perspective_Camera cam_cctv_1{ CAMERA_CCTV_1 };
	Perspective_Camera cam_cctv_2{ CAMERA_CCTV_2 };
	Perspective_Camera cam_cctv_dynamic{ CAMERA_CCTV_DYNAMIC }; // 동적 CCTV

	Orthographic_Camera cam_front_ortho{ CAMERA_FRONT_ORTHO }; // 정면도
	Orthographic_Camera cam_side_ortho{ CAMERA_SIDE_ORTHO };   // 측면도
	Orthographic_Camera cam_top_ortho{ CAMERA_TOP_ORTHO };     // 상면도
};
