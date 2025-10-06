#include "ShaderLibrary.h"

ShaderLibrary& ShaderLibrary::Get()
{
	static ShaderLibrary instance;
	return instance;
}

void ShaderLibrary::AddShader(const std::string& name, const std::string& vertpath, const std::string& fragpath)
{
	if (ContainsShader(name)) {
		std::cerr << "Warning the shader" + name + "already exists! ";
	}

	try {
		auto shader = std::make_unique<Shader>(vertpath.c_str(), fragpath.c_str());
		shaders[name] = std::move(shader);
	}
	catch (const std::exception e) {
		std::cerr << "Failed to create the shader" + name;
		throw;
	}
}

Shader& ShaderLibrary::GetShader(const std::string& name)
{
	auto it = shaders.find(name);
	if (it == shaders.end()) {
		std::cerr << "Shader " + name + " not found in library";
	}
	return *(it->second);
}

bool ShaderLibrary::ContainsShader(const std::string& name)
{
	return shaders.find(name) != shaders.end();
}
