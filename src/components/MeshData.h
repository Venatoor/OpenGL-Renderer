#pragma once
#include <vector>
#include <string>   
#include <memory>   

#include <glm/glm.hpp> 
#include "shaderClass.h" 
#include "Texture.h"     
#include "VAO.h"         
#include "VBO.h"         
#include "EBO.h"         


struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class MeshData {
public:

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    VAO vao;
    VBO vbo;
    EBO ebo;

    MeshData( const std::vector<Vertex>& vertices,
         const std::vector<unsigned int>& indices);


    ~MeshData() = default; 

    void SetupBuffers();

    const std::vector<Vertex>& GetVertices() { return vertices; }
    const std::vector<unsigned int>& GetIndices() { return indices; }

    void Draw();
};