#pragma once
#include <glm/glm.hpp>
#include "RayTracingStructs.h"
#include "MeshData.h"

struct RayTracingMaterial
{
	glm::vec3 albedo = glm::vec3(0.8f);
	float roughness = 0.5f;
	bool emissive = false;
	float ior = 1.0f;
	float transparency = 0.0f;

	ComputeMaterial SendToGPU() const {
		return ComputeMaterial{
			albedo,
			1.0f-roughness,
			emissive,
			ior,
			transparency
		};
	}

};

