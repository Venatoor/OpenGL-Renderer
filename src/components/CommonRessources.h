#pragma once
#include "MeshData.h"
#include <memory>
#include <vector>

struct CommonRessources {

	static std::shared_ptr<MeshData> GetCubeMesh();
	static std::shared_ptr<MeshData> GetVerticalQuadMesh();
	static std::shared_ptr<MeshData> GetPlaneMesh();

private:

	static std::shared_ptr<MeshData> cubeMesh;
	static std::shared_ptr<MeshData> verticalQuadMesh;
	static std::shared_ptr<MeshData> planeMesh;

};

