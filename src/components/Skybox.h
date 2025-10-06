#pragma once
#include "Cubemap.h"
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include <glm/glm.hpp>

class Skybox
{
public:

	Skybox(const std::vector<std::string>& cubemapFaces, Shader& shader);
	~Skybox();

	void Draw(const glm::mat4& view, const glm::mat4& projection);

	const Cubemap& GetCubemap() const { }

	const int& GetCubeMapID();

private:

	void SetupSkyboxGeometry();

	Cubemap skyboxCubemap;
	Shader skyboxShader;
	VAO vao;
	VBO vbo;

	GLuint skyboxVAO;
	GLuint skyboxVBO;
};

