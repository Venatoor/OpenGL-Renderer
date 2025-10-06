#include "Mesh.h"
#include <glad/glad.h>

Mesh::Mesh(std::shared_ptr<MeshData> sharedData) :
	meshData(std::move(sharedData))
{
}


void Mesh::OnRender(Shader& shader, const glm::mat4& modelMatrix)
{

	shader.SetMat4("model", modelMatrix);
	material->BindTextures(shader);
	meshData->Draw();
}
