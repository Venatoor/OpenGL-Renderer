#include "CommonRessources.h"

std::shared_ptr<MeshData> CommonRessources::cubeMesh = nullptr;
std::shared_ptr<MeshData> CommonRessources::planeMesh = nullptr;
std::shared_ptr<MeshData> CommonRessources::verticalQuadMesh = nullptr;

std::shared_ptr<MeshData> CommonRessources::GetCubeMesh()
{
    if (!cubeMesh) {
        std::vector<Vertex> vertices = 
        {
            // Positions          // Normals           // Texture Coords
            // Front face
            {{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // 0
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // 1
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // 2
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // 3
            // Back face
            {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}}, // 4
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}}, // 5
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}}, // 6
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}}, // 7
            // Top face
            {{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}, // 8  (same as 3)
            {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}, // 9  (same as 2)
            {{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}}, // 10 (same as 6)
            {{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}, // 11 (same as 7)
            // Bottom face
            {{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}}, // 12 (same as 0)
            {{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}}, // 13 (same as 1)
            {{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}}, // 14 (same as 5)
            {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}}, // 15 (same as 4)
            // Right face
            {{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 16 (same as 1)
            {{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 17 (same as 5)
            {{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}, // 18 (same as 6)
            {{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 19 (same as 2)
            // Left face
            {{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 20 (same as 0)
            {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // 21 (same as 4)
            {{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}}, // 22 (same as 7)
            {{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}}  // 23 (same as 3)
        };

        std::vector<unsigned int> indices = {
            // Front face
            0, 1, 2,
            2, 3, 0,
            // Back face
            5, 4, 7,
            7, 6, 5,
            // Top face
            8, 9, 10,
            10, 11, 8,
            // Bottom face
            14, 13, 12,
            12, 15, 14,
            // Right face
            16, 17, 18,
            18, 19, 16,
            // Left face
            20, 21, 22,
            22, 23, 20
        };

        cubeMesh = std::make_shared<MeshData>(vertices, indices);
    }
    return cubeMesh;
}

std::shared_ptr<MeshData> CommonRessources::GetVerticalQuadMesh()
{
    if (!verticalQuadMesh) {
        std::vector<Vertex> vertices = {
            // Positions              // Normals           // Texture Coords
            {{-1.0f, -1.0f, 0.0f},   {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f}}, // 0 - bottom left
            {{ 1.0f, -1.0f, 0.0f},   {0.0f, 0.0f, 1.0f},  {1.0f, 0.0f}}, // 1 - bottom right
            {{ 1.0f,  1.0f, 0.0f},   {0.0f, 0.0f, 1.0f},  {1.0f, 1.0f}}, // 2 - top right
            {{-1.0f,  1.0f, 0.0f},   {0.0f, 0.0f, 1.0f},  {0.0f, 1.0f}}  // 3 - top left
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0
        };

        verticalQuadMesh = std::make_shared<MeshData>(vertices, indices);
    }
    return verticalQuadMesh;
}

std::shared_ptr<MeshData> CommonRessources::GetPlaneMesh()
{
    if (!planeMesh) {
        std::vector<Vertex> vertices = {
            // Positions              // Normals           // Texture Coords
            {{-1.0f, 0.0f, -1.0f},   {0.0f, 1.0f, 0.0f},  {0.0f, 0.0f}}, // 0 - bottom left
            {{ 1.0f, 0.0f, -1.0f},   {0.0f, 1.0f, 0.0f},  {1.0f, 0.0f}}, // 1 - bottom right
            {{ 1.0f, 0.0f, 1.0f},   {0.0f, 1.0f, 0.0f},  {1.0f, 1.0f}}, // 2 - top right
            {{-1.0f, 0.0f, 1.0f},   {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f}}  // 3 - top left
        };

        std::vector<unsigned int> indices = {
            0, 1, 2,
            2, 3, 0
        };

        planeMesh = std::make_shared<MeshData>(vertices, indices);
    }
    return planeMesh;
}
