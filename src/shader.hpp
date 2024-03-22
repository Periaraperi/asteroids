#pragma once

#include <cstdint>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

class Shader {
public:
	Shader(const std::string& vertex_path, const std::string& fragment_path);
	Shader(Shader&&) = default;
	Shader& operator=(Shader&&) = default;
	~Shader();

	void bind() const;
	void unbind() const;

    void set_int(const std::string& u_name, int val) const;
	void set_float(const std::string& u_name, float val) const;
	void set_vec2(const std::string& u_name, const glm::vec2& v) const;
	void set_vec3(const std::string& u_name, const glm::vec3& v) const;
	void set_vec4(const std::string& u_name, const glm::vec4& v) const;
    void set_mat4(const std::string& u_name, const glm::mat4& m) const;
    void set_array(const std::string& u_name, int count, int* arr) const;

private:
	uint32_t _id;
	std::string parse_shader(const std::string& path);
	uint32_t compile_shader(const std::string& src, uint32_t type);
	void create_shader_program(uint32_t vertex_shader, uint32_t fragment_shader);

public: // disable copy operations
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
};
