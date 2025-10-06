#include "Model.h"
#include "stb/stb_image.h"


Model::Model(const char* path) {
	std::cout << "=== Loading model: " << path << " ===\n";
	LoadModel(path);
	std::cout << "=== Model loaded with " << meshes.size() << " meshes ===\n\n";
}


void Model::LoadModel(const std::string& path) {

	Assimp::Importer importer;
	unsigned int flags = aiProcess_Triangulate
		| aiProcess_CalcTangentSpace;

	const aiScene* scene = importer.ReadFile(path, flags);

	if (!scene || !scene->mRootNode 
		|| scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE 
		|| !scene->mRootNode) {
		std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	std::cout << "Model directory: " << directory << "\n";

	ProcessNode(scene->mRootNode, scene);

}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
	for (unsigned i = 0; i < node->mNumMeshes; i++) {

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	for (unsigned i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {

	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;


	//Processing Vertices
	for (unsigned i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		vertex.position = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);

		vertex.normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);

		if (mesh->mTextureCoords[0]) {
			vertex.texCoord = glm::vec2{
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			};
		}
		else {
			vertex.texCoord = glm::vec2(0.0f);
		}


		if (mesh->HasTangentsAndBitangents()) {
			vertex.tangent = glm::vec3(
				mesh->mTangents[i].x,
				mesh->mTangents[i].y,
				mesh->mTangents[i].z
			);

			vertex.bitangent = glm::vec3(
				mesh->mBitangents[i].x,
				mesh->mBitangents[i].y,
				mesh->mBitangents[i].z
			);
		}
		else {
			vertex.tangent = glm::vec3(1.0f, 0.0f, 0.0f);
			vertex.bitangent = glm::vec3(0.0f, 1.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	std::cout << "\nUV Coordinate Report:\n";
	std::cout << "Min UV: (" << std::numeric_limits<float>::max() << "," << std::numeric_limits<float>::max() << ")\n";
	std::cout << "Max UV: (" << -std::numeric_limits<float>::max() << "," << -std::numeric_limits<float>::max() << ")\n";

	float minU = std::numeric_limits<float>::max();
	float minV = std::numeric_limits<float>::max();
	float maxU = -std::numeric_limits<float>::max();
	float maxV = -std::numeric_limits<float>::max();

	for (const auto& vertex : vertices) {
		minU = std::min(minU, vertex.texCoord.x);
		minV = std::min(minV, vertex.texCoord.y);
		maxU = std::max(maxU, vertex.texCoord.x);
		maxV = std::max(maxV, vertex.texCoord.y);
	}

	std::cout << "Actual Min UV: (" << minU << "," << minV << ")\n";
	std::cout << "Actual Max UV: (" << maxU << "," << maxV << ")\n";


	//Processing Indices 
	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	
	std::shared_ptr<MeshData> sharedMeshData =
		std::make_shared<MeshData>(vertices, indices);


	return Mesh(sharedMeshData);
}
