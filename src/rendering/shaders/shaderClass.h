#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include <glm/fwd.hpp>
#include <unordered_set>


std::string get_file_contents(const char* fileName);

class Shader {
public : 
	GLuint ID;

	// Rule of five element
	Shader(const char* vertexFile, const char* fragmentFile);
	Shader(const char* vertexFile, const char* fragmentFile, const char* geometryFile);
	Shader(const char* vertexFile, const char* fragmentFile, const char* tessControlFile, const char* tessEvalFile);
	Shader(const char* vertexFile, const char* fragmentFile, 
		   const char* geometryFile, const char* tessControlFile, const char* tessEvalFile);
	Shader(const std::vector<std::pair<GLenum, const char* >>& shaders);
	//Shader(const Shader&) = delete;
	//Shader& operator=(const Shader&) = delete;
	//Shader(Shader&& other) noexcept;
	//Shader& operator=(Shader&& other) noexcept;
	//~Shader();

	//Lifetime
	void Activate();
	void Delete();

	// Uniform setters
	void SetMat4(const std::string& name, const glm::mat4& mat) const;
	//void SetMat3(const std::string& name, const glm::mat3& mat) const;
	//void SetVec4(const std::string& name, const glm::vec4& value) const;
	//void SetVec2(const std::string& name, const glm::vec2& value) const;
	void SetVec3(const std::string& name, const glm::vec3& value) const;
	void SetFloat(const std::string& name, float value) const;
	void SetBool(const std::string& name, bool value) const;
	void SetInt(const std::string& name, int value) const;
	void SetUInt(const std::string& name, uint32_t value) const;

	//Array uniforms
	//void SetIntArray(const std::string& name, const int* values, uint32_t count) const;
	//void SetFloatArray(const std::string& name, const float* values, uint32_t count) const;
	//void SetMat4Array(const std::string& name, const glm::mat4* mats, uint32_t count) const;

private:

	//static std::string PreprocessShader(const std::string& source, const std::string& includeDir = "");
	std::string PreProcessShader(const std::string& filePath,
		std::unordered_set<std::string>& includedFiles,
		const std::string& baseDir);
	std::string GetDirectoryFromPath(const std::string& filepath);

	GLuint CompileShader(GLenum type, const char* source, const char* shaderType);
	void LinkProgram(const std::vector<GLuint>& shaders);
	void CreateFromFiles(const std::vector<std::pair<GLenum, const char*>>& shaders);

};

#endif

