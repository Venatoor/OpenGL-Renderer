#pragma once
#include <glm/glm.hpp>
#include "MeshData.h"

	// NB : Vertex from MeshData is also used here

	struct ComputeMaterial {
		glm::vec3 color;
		float smoothness;
		bool emissive;
		float refraction;
		float transparency;
	};

	struct Ray {
		glm::vec3 origin;
		glm::vec3 direction;
		glm::vec3 color;
	};

	struct RayHitInfo {
		glm::vec3 position;
		glm::vec3 normal;
		float depth;
		bool hit;
		ComputeMaterial material;
	};

	struct MeshInfo {
		int startIndex;
		int vertexCount;
	};

	struct MeshInstanceCommand {

		int meshIndex;
		float transform[16];
		ComputeMaterial material;

	};
