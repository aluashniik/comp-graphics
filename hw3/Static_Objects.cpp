#define _CRT_SECURE_NO_WARNINGS

#include "Scene_Definitions.h"

void Static_Object::read_geometry(int bytes_per_primitive) {
	FILE* fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: cannot open the object file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	vertices = (float*)malloc(n_triangles * bytes_per_primitive);
	if (vertices == NULL) {
		fprintf(stderr, "Error: cannot allocate memory for the geometry file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(vertices, bytes_per_primitive, n_triangles, fp); // assume the data file has no faults.
	// fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);
}

void Static_Object::prepare_geom_of_static_object() {
	int i, n_bytes_per_vertex, n_bytes_per_triangle;
	char filename[512];

	n_bytes_per_vertex = n_fields * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	read_geometry(n_bytes_per_triangle);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, n_triangles * n_bytes_per_triangle, vertices, GL_STATIC_DRAW);

	free(vertices);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Building::define_object() {
	Material* cur_material;
	strcpy(filename, "Data/Building1_vnt.geom");
	n_fields = 8;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	instances.back().ModelMatrix = glm::mat4(1.0f);

	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.135f, 0.2225f, 0.1575f, 1.0f);
	cur_material->diffuse = glm::vec4(0.54f, 0.89f, 0.63f, 1.0f);
	cur_material->specular = glm::vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
	cur_material->exponent = 128.0f * 0.1f;
}


void Light::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/Light_vn.geom");
	n_fields = 6;
	front_face_mode = GL_CCW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 100.0f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 47.5f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 130.0f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(190.0f, 60.0f, 49.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 112.5f, 49.0));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	cur_material->diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	cur_material->specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	cur_material->exponent = 128.0f * 0.4f;
}

void Dragon::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/dragon_vnt.geom"); // 경로 확인
	n_fields = 8;
	front_face_mode = GL_CW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(152.0f, 32.5f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.7f, 0.7f, 0.7f)); // 크기


	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.6f, 0.6f, 0.2f, 1.0f);
	cur_material->diffuse = glm::vec4(1.0f, 1.0f, 0.4f, 1.0f);
	cur_material->specular = glm::vec4(1.0f, 1.0f, 0.8f, 1.0f);
	cur_material->exponent = 11.334717f;
}

void Optimus::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/optimus_vnt.geom"); // 경로 확인
	n_fields = 8;
	front_face_mode = GL_CW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);

	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(35.0f, 38.0f, 0.0f)); 
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.06f, 0.06f, 0.06f)); // 크기

	*cur_MM = glm::rotate(*cur_MM, -45.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	//*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	
	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	cur_material->diffuse = glm::vec4(0.123f, 0.456f, 0.789f, 1.0f);
	cur_material->specular = glm::vec4(0.3f, 0.3f, 0.4f, 1.0f);
	cur_material->exponent = 11.334717f;
}

void Godzilla::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/godzilla_vnt.geom"); // 경로 확인
	n_fields = 8;
	front_face_mode = GL_CW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 56.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(0.17f, 0.17f, 0.17f)); // 크기
	*cur_MM = glm::rotate(*cur_MM, 135.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	cur_material->diffuse = glm::vec4(0.568f, 0.324f, 0.414f, 1.0f);
	cur_material->specular = glm::vec4(0.3f, 0.3f, 0.4f, 1.0f);
	cur_material->exponent = 11.334717f;
}

void Cat::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/cat_vnt.geom"); 
	n_fields = 8;
	front_face_mode = GL_CW;
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(214.0f, 25.0f, 0.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(17.0f, 17.0f, 17.0f)); // 크기
	*cur_MM = glm::rotate(*cur_MM, -90.0f * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	*cur_MM = glm::rotate(*cur_MM, 90.0f * TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	cur_material = &(instances.back().material);
	cur_material->emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	cur_material->ambient = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f); // 주변광
	cur_material->diffuse = glm::vec4(0.6f, 0.6f, 0.6f, 1.0f); // 회색
	cur_material->specular = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f); // 반사광
	cur_material->exponent = 11.334717f;
}

void Helicopter::define_object() {
	glm::mat4* cur_MM;
	Material* cur_material;
	strcpy(filename, "Data/static_objects/helicopter_vntm.geom"); // 경로 확인
	n_fields = 8;
	front_face_mode = GL_CW; 
	prepare_geom_of_static_object();
	flag_valid = true;

	instances.emplace_back();
	cur_MM = &(instances.back().ModelMatrix);
	*cur_MM = glm::translate(glm::mat4(1.0f), glm::vec3(150.0f, 100.0f, 57.0f));
	*cur_MM = glm::scale(*cur_MM, glm::vec3(3.5f, 3.5f, 3.5f)); 

	*cur_MM = glm::rotate(*cur_MM, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // X축 90도 회전 예시
	// *cur_MM = glm::rotate(*cur_MM, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f)); // Z축 45도 회전 예시

	cur_material = &(instances.back().material);
	cur_material->ambient = glm::vec4(0.6f, 0.3f, 0.0f, 1.0f);   // 주변광
	cur_material->diffuse = glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);   // 선명한 주황색
	cur_material->specular = glm::vec4(1.0f, 0.8f, 0.5f, 1.0f);
	cur_material->exponent = 11.334717f; 
}

/////////////////////////////////////
void print_mat4(const char* string, glm::mat4 M) {
	fprintf(stdout, "\n***** %s ******\n", string);
	for (int i = 0; i < 4; i++)
		fprintf(stdout, "*** COL[%d] (%f, %f, %f, %f)\n", i, M[i].x, M[i].y, M[i].z, M[i].w);
	fprintf(stdout, "**************\n\n");
}
void Static_Object::draw_object(glm::mat4& ViewMatrix, glm::mat4& ProjectionMatrix, SHADER_ID shader_kind,
	std::vector<std::reference_wrapper<Shader>>& shader_list) {
	glm::mat4 ModelViewProjectionMatrix;
	glFrontFace(front_face_mode);
	for (int i = 0; i < instances.size(); i++) {
		ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * instances[i].ModelMatrix;
		switch (shader_kind) {
		case SHADER_SIMPLE:
			Shader_Simple* shader_simple_ptr = static_cast<Shader_Simple*>(&shader_list[shader_ID_mapper[shader_kind]].get());
			glUseProgram(shader_simple_ptr->h_ShaderProgram);
			glUniformMatrix4fv(shader_simple_ptr->loc_ModelViewProjectionMatrix, 1, GL_FALSE,
				&ModelViewProjectionMatrix[0][0]);
			glUniform3f(shader_simple_ptr->loc_primitive_color, instances[i].material.diffuse.r,
				instances[i].material.diffuse.g, instances[i].material.diffuse.b);
			break;
		}
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3 * n_triangles);
		glBindVertexArray(0);
		glUseProgram(0);
	}
}
