#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

//this is catastrophic lol
std::string get_file_contents_duplicate(const char* fileName);

class ComputeShader
{
public:

	//ROF
	ComputeShader(const char* computeFile);
	ComputeShader(const ComputeShader&) = delete;
	ComputeShader& operator=(const ComputeShader&) = delete;
	ComputeShader(ComputeShader&& other) noexcept;
	ComputeShader& operator=(ComputeShader&& other) noexcept;
	~ComputeShader();

	//DISPATCHING
	void Dispatch(glm::uvec3 workGroups) const;
	void DispatchIndirect(GLuint commandBuffer) const;

	//MEMORY BARRIER SET
	static void SetMemoryBarriers(GLbitfield barriers);

	void SetUniform(const std::string& name, int value);
	void SetUniform(const std::string& name, float value);
	void SetUniform(const std::string& name, const glm::vec2& value);
	void SetUniform(const std::string& name, const glm::vec3& value);
	void SetUniform(const std::string& name, const glm::vec4& value);
	void SetUniform(const std::string& name, const glm::uvec3& value);
	void SetUniform(const std::string& name, const glm::mat3& value);
	void SetUniform(const std::string& name, const glm::mat4& value);


	//RESSOURCE BINDING
	void Bind();
	static void Unbind();
	GLuint GetProgramID() const { return program; }
	bool IsValid() const { return program != 0; }

private:

	ComputeShader() = default;
	GLuint CompileShader(const std::string& source);
	GLint GetUniformLocation(const std::string& name);

	GLuint program = 0;
	mutable std::unordered_map<std::string, GLint> uniformLocationCache;


};

