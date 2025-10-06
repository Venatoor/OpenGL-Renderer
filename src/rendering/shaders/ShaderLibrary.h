#pragma once
#include <unordered_map>
#include <memory>
#include "shaderClass.h"

class ShaderLibrary
{
public:
	static ShaderLibrary& Get();

	void AddShader(const std::string& name, const std::string& vertpath, const std::string& fragpath);

	Shader& GetShader(const std::string& name);
	bool ContainsShader(const std::string& name);

private:

	ShaderLibrary() = default;
	std::unordered_map<std::string, std::unique_ptr<Shader>> shaders;
};

