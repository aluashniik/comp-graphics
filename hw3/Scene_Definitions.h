#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Shaders/LoadShaders.h"
#include "Camera.h"
#include <map>
#include <functional>

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))
#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f

#define N_MAX_STATIC_OBJECTS		10
#define N_MAX_DYNAMIC_OBJECTS		10
#define N_MAX_CAMERAS		10
#define N_MAX_SHADERS		10

extern unsigned int static_object_ID_mapper[N_MAX_STATIC_OBJECTS];
extern unsigned int dynamic_object_ID_mapper[N_MAX_DYNAMIC_OBJECTS];
extern unsigned int camera_ID_mapper[N_MAX_CAMERAS];
extern unsigned int shader_ID_mapper[N_MAX_SHADERS];

enum STATIC_OBJECT_ID {
	STATIC_OBJECT_BUILDING = 0, STATIC_OBJECT_TABLE, STATIC_OBJECT_LIGHT, 
	STATIC_OBJECT_TEAPOT, STATIC_OBJECT_NEW_CHAIR, 
	STATIC_OBJECT_FRAME, STATIC_OBJECT_NEW_PICTURE,

	STATIC_OBJECT_DRAGON, STATIC_OBJECT_CAT, STATIC_OBJECT_HELICOPTER, STATIC_OBJECT_OPTIMUS , STATIC_OBJECT_GODZILLA
};

enum DYNAMIC_OBJECT_ID {
	DYNAMIC_OBJECT_TIGER=0, DYNAMIC_OBJECT_BEN, DYNAMIC_OBJECT_SPIDER
};

enum SHADER_ID { SHADER_SIMPLE = 0, SHADER_PHONG, SHADER_PHONG_TEXUTRE };

struct Shader {
	ShaderInfo shader_info[3];
	GLuint h_ShaderProgram; // handle to shader program
	GLint loc_ModelViewProjectionMatrix;

	Shader() {
		h_ShaderProgram = NULL;
		loc_ModelViewProjectionMatrix = -1;
		shader_info[0] = shader_info[1] = shader_info[2] = { NULL, NULL };
	}
	virtual void prepare_shader() = 0;
};

struct Shader_Simple : Shader {
	GLint loc_primitive_color;
	void prepare_shader();
};

struct Shader_Data {
	Shader_Simple shader_simple;
	// Shader_Phong shader_phong;
	// Shader_Phong_Texture Shader_Phong_texture;
};

struct Material {
	glm::vec4 emission, ambient, diffuse, specular;
	GLfloat exponent;
};

struct Instance {
	glm::mat4 ModelMatrix;
	Material material;
};

struct Axis_Object {
	GLuint VBO, VAO;
	GLfloat vertices_axes[6][3] = {
		{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f }
	};
	GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };

	void define_axis();
	void draw_axis(Shader_Simple* shader_simple, glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix);
};

struct Static_Object { // an object that does not move
	STATIC_OBJECT_ID object_id;
	char filename[512]; // where to read geometry data
	int n_fields; // 3/6/8 where3 floats for vertex, 3 floats for normal, and 2 floats for texcoord
	int n_triangles;
	GLfloat* vertices; // pointer to vertex array data
	GLuint VBO, VAO; // handles to vertex buffer object and vertex array object
	GLenum front_face_mode; // clockwise or counter-clockwise

	std::vector<Instance> instances;
	bool flag_valid;

	Static_Object() {}
	Static_Object(STATIC_OBJECT_ID _object_id) : object_id(_object_id) {
		instances.clear();
	}
	void read_geometry(int bytes_per_primitive);  
	void prepare_geom_of_static_object();
	void draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
		std::vector<std::reference_wrapper<Shader>>& shader_list);
};

struct Building : public Static_Object { 
	Building(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};


struct Light : public Static_Object { 
	Light(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object(); 
};

struct Dragon:public Static_Object{
	Dragon(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};

struct Optimus :public Static_Object {
	Optimus(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};
struct Godzilla :public Static_Object {
	Godzilla(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
}; 
struct Cat :public Static_Object {
	Cat(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};

struct Helicopter :public Static_Object {
	Helicopter(STATIC_OBJECT_ID _object_id) : Static_Object(_object_id) {}
	void define_object();
};


struct Static_Geometry_Data {
	Building building{ STATIC_OBJECT_BUILDING };
	//Table table{ STATIC_OBJECT_TABLE };
	Light light{ STATIC_OBJECT_LIGHT };
	//////////////
	Dragon dragon{ STATIC_OBJECT_DRAGON };
	Optimus optimus{STATIC_OBJECT_OPTIMUS};
	Godzilla godzilla{ STATIC_OBJECT_GODZILLA};
	Cat cat{ STATIC_OBJECT_CAT };
	Helicopter helicopter{ STATIC_OBJECT_HELICOPTER };
};

struct Dynamic_Object { // an object that moves
	DYNAMIC_OBJECT_ID object_id;
	std::vector<Static_Object> object_frames;
	bool flag_valid;

	Dynamic_Object() {}
	Dynamic_Object(DYNAMIC_OBJECT_ID _object_id) : object_id(_object_id) {
		object_frames.clear();
	}

	void draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
		std::vector<std::reference_wrapper<Shader>>& shader_list, int time_stamp);
};

struct Ben_D : public Dynamic_Object {
	Ben_D(DYNAMIC_OBJECT_ID _object_id):Dynamic_Object(_object_id){}
	void define_object();
};

struct Spider_D : public Dynamic_Object {
	Spider_D(DYNAMIC_OBJECT_ID _object_id):Dynamic_Object(_object_id) {}
	void define_object();
};

struct Dynamic_Geometry_Data {
	Ben_D ben_d{DYNAMIC_OBJECT_BEN};
	Spider_D spider_d{ DYNAMIC_OBJECT_SPIDER };
};

struct Window {
	int width, height;
	float aspect_ratio;
};



struct Scene {
	unsigned int time_stamp;
	Static_Geometry_Data static_geometry_data;
	std::vector<std::reference_wrapper<Static_Object>> static_objects;

	Dynamic_Geometry_Data dynamic_geometry_data;
	std::vector<std::reference_wrapper<Dynamic_Object>> dynamic_objects;

	Camera_Data camera_data;
	std::vector<std::reference_wrapper<Camera>> camera_list;

	Shader_Data shader_data;
	std::vector<std::reference_wrapper<Shader>> shader_list;
	SHADER_ID shader_kind;

	Window window; // for a better code, this must be defined in another structure!

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;

	Axis_Object axis_object;

	bool display_cctv_static = true;    // True: show static CCTV, False: show orthographic views
	bool display_ortho_views = false;   // True: show orthographic views, False: not shown by default


	Scene() {
		time_stamp = 0;
		static_objects.clear();
		shader_list.clear();
		shader_kind = SHADER_SIMPLE;
		ViewMatrix = ProjectionMatrix = glm::mat4(1.0f);
	}

	void clock(int clock_id);
	void build_static_world();
	void build_dynamic_world();
	void create_camera_list(int win_width, int win_height, float win_aspect_ratio);
	void build_shader_list();
	void initialize();
	void draw_static_world();
	void draw_dynamic_world();
	void draw_axis();
	void draw_world();
};






	 
 