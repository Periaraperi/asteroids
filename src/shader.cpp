#include "shader.hpp"

#include "opengl_errors.hpp"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sstream>
#include <fstream>

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path)
{
	std::string vertex_src = parse_shader(vertex_path);
	std::string fragment_src = parse_shader(fragment_path);

	uint32_t vertex_shader = compile_shader(vertex_src, GL_VERTEX_SHADER);
	uint32_t fragment_shader = compile_shader(fragment_src, GL_FRAGMENT_SHADER);

	create_shader_program(vertex_shader, fragment_shader);
	GL_CALL(glDeleteShader(vertex_shader));
	GL_CALL(glDeleteShader(fragment_shader));
    
    std::cerr << "Shader ctor()\n";
}

Shader::~Shader()
{
	GL_CALL(glDeleteProgram(_id));

    std::cerr << "Shader dtor()\n";
}

// TODO: add logic if invalid path or can't open file
std::string Shader::parse_shader(const std::string& path)
{
	std::ifstream shader_file;

	shader_file.open(path.c_str());

	std::stringstream ss;
	ss << shader_file.rdbuf();

	shader_file.close();

	return ss.str();
}

uint32_t Shader::compile_shader(const std::string& src, uint32_t type)
{
	uint32_t shader = glCreateShader(type);
	const char* shader_src = src.c_str();
	GL_CALL(glShaderSource(shader,1,&shader_src,nullptr));
	GL_CALL(glCompileShader(shader));

	// check if successfully compiled
	int success;
	char log[512];
	GL_CALL(glGetShaderiv(shader,GL_COMPILE_STATUS,&success));
	if(!success) {
		GL_CALL(glGetShaderInfoLog(shader,512,nullptr,log));
		std::cerr << "Couldn't compile shader\n" << log << '\n';
	}

	return shader;
}

void Shader::create_shader_program(uint32_t vertex_shader, uint32_t fragment_shader)
{
	_id = glCreateProgram();
	GL_CALL(glAttachShader(_id,vertex_shader));
	GL_CALL(glAttachShader(_id,fragment_shader));
	GL_CALL(glLinkProgram(_id));

	// check if successfully linked
	int success;
	char log[512];
	GL_CALL(glGetProgramiv(_id,GL_LINK_STATUS,&success));
	if(!success) {
		GL_CALL(glGetProgramInfoLog(_id,512,nullptr,log));
		std::cerr << "Couldn't link shader program\n" << log << '\n';
	}
}

void Shader::bind() const
{ GL_CALL(glUseProgram(_id)); }

void Shader::unbind() const
{ GL_CALL(glUseProgram(0)); }

void Shader::set_int(const std::string& u_name, int val) const
{ GL_CALL(glUniform1i(glGetUniformLocation(_id,u_name.c_str()),val)); }

void Shader::set_float(const std::string& u_name, float val) const
{ GL_CALL(glUniform1f(glGetUniformLocation(_id,u_name.c_str()),val)); }

void Shader::set_vec2(const std::string& u_name, const glm::vec2& v) const 
{ GL_CALL(glUniform2f(glGetUniformLocation(_id,u_name.c_str()),v.x,v.y)); }

void Shader::set_vec3(const std::string& u_name, const glm::vec3& v) const
{ GL_CALL(glUniform3f(glGetUniformLocation(_id,u_name.c_str()),v.x,v.y,v.z)); }

void Shader::set_vec4(const std::string& u_name, const glm::vec4& v) const
{ GL_CALL(glUniform4f(glGetUniformLocation(_id,u_name.c_str()),v.x,v.y,v.z,v.w)); }

void Shader::set_mat4(const std::string& u_name, const glm::mat4& m) const
{ GL_CALL(glUniformMatrix4fv(glGetUniformLocation(_id,u_name.c_str()),1,GL_FALSE,glm::value_ptr(m))); }

void Shader::set_array(const std::string& u_name, int count, int* arr) const
{ GL_CALL(glUniform1iv(glGetUniformLocation(_id,u_name.c_str()),count,arr)); }
