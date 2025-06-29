#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"


// ben object
#define N_BEN_FRAMES 30

void Ben_D::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	flag_valid = true;

	for (int i = 0; i < N_BEN_FRAMES; i++) {
		object_frames.emplace_back();
		sprintf(object_frames[i].filename, "Data/dynamic_objects/ben/ben_vntm_%02d.geom", i);
		object_frames[i].n_fields = 8;
		object_frames[i].front_face_mode = GL_CW;
		object_frames[i].prepare_geom_of_static_object();

		object_frames[i].instances.emplace_back();
		cur_MM = &(object_frames[i].instances.back().ModelMatrix);
		*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(44.0f, 44.0f, 44.0f)); // 적절한 크기 조정
		// *cur_MM = glm::rotate(*cur_MM, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X축 회전 예시

		cur_material = &(object_frames[i].instances.back().material);
		cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		cur_material->ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		cur_material->diffuse = glm::vec4(0.9882f, 0.2745f, 0.4627f, 1.0f);
		cur_material->specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		cur_material->exponent = 128.0f * 0.21794872f;
	}
}

//spider object
#define N_SPIDER_FRAMES 16

void Spider_D::define_object() {
		glm::mat4* cur_MM;
		Material* cur_material;
		flag_valid = true;

		for (int i = 0; i < N_SPIDER_FRAMES; i++) {
			object_frames.emplace_back(); 
			sprintf(object_frames[i].filename, "Data/dynamic_objects/spider/spider_vnt_%02d.geom", i); 
			object_frames[i].n_fields = 8;
			object_frames[i].front_face_mode = GL_CW; 
			object_frames[i].prepare_geom_of_static_object();

			object_frames[i].instances.emplace_back();
			cur_MM = &(object_frames[i].instances.back().ModelMatrix);
			*cur_MM = glm::scale(glm::mat4(1.0f), glm::vec3(15.0f, 10.5f, 15.0f));
			*cur_MM = glm::translate(*cur_MM, glm::vec3(0.0f, 0.0f, 0.0f)); 

			cur_material = &(object_frames[i].instances.back().material);
			cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			cur_material->ambient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);   
			cur_material->diffuse = glm::vec4(0.541f, 0.152f, 0.321f, 1.0f);   
			cur_material->specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f); 
			cur_material->exponent = 11.334717f;                       
		}
}
void Dynamic_Object::draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
	std::vector<std::reference_wrapper<Shader>>& shader_list, int time_stamp) {
	int cur_object_index = time_stamp % object_frames.size();
	Static_Object& cur_object = object_frames[cur_object_index];
	glFrontFace(cur_object.front_face_mode);

	float rotation_angle = 0.0f;
	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	switch (object_id) {
	case DYNAMIC_OBJECT_BEN:
	{
		float ben_x_pos = 128.0f + 40.0f * sin(time_stamp * 0.07f); // x축을 따라 
		float ben_y_pos = 146.0f; // y축 고정
		float ben_z_pos = 0.0f; // 바닥에 붙어있게
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(ben_x_pos, ben_y_pos, ben_z_pos));
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X축 회전
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	} 
	break;
	case DYNAMIC_OBJECT_SPIDER:
	{ 
		float spider_y_pos = 85.0f + 20.0f * cos(time_stamp * 0.15f); 
		float spider_x_pos = 210.0;
		float spider_z_pos = 15.0f; 
		ModelMatrix = glm::translate(ModelMatrix, glm::vec3(spider_x_pos, spider_y_pos, spider_z_pos));
		rotation_angle = atan2(30.0f * cos(time_stamp * 0.1f) * 0.1f, -30.0f * sin(time_stamp * 0.1f) * 0.1f);
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		ModelMatrix = glm::rotate(ModelMatrix, rotation_angle + glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	} // 
	break;
}

	for (int i = 0; i < cur_object.instances.size(); i++) {
		glm::mat4 ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix * cur_object.instances[i].ModelMatrix;
		switch (shader_kind) {
		case SHADER_SIMPLE:
			Shader_Simple* shader_simple_ptr = static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(shader_simple_ptr->h_ShaderProgram);
			glUniformMatrix4fv(shader_simple_ptr->loc_ModelViewProjectionMatrix, 1, GL_FALSE,
				&ModelViewProjectionMatrix[0][0]);
			glUniform3f(shader_simple_ptr->loc_primitive_color, cur_object.instances[i].material.diffuse.r,
				cur_object.instances[i].material.diffuse.g, cur_object.instances[i].material.diffuse.b);
			break;
		}
		glBindVertexArray(cur_object.VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * cur_object.n_triangles);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}