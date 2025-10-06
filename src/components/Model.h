#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include"Texture.h"
#include"shaderClass.h"
#include"Mesh.h"
#include"Texture.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
public:
	Model(const char* path);

	std::vector<Mesh> meshes;
private:

	
	std::string directory;	
	void LoadModel(const std::string& path);

	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

};


