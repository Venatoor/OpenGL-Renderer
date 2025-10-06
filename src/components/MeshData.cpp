#include "MeshData.h"
#include <glad/glad.h> 
#include <cstddef>     
#include <utility>     

MeshData::MeshData( const std::vector<Vertex>& verts,
     const std::vector<unsigned int>& idx)
    : vertices(std::move(verts)),
    indices(std::move(idx)),
    vao(), 
    vbo(reinterpret_cast<GLfloat*>(this->vertices.data()), this->vertices.size() * sizeof(Vertex)),
    ebo(this->indices.data(), this->indices.size() * sizeof(unsigned int))
{
    SetupBuffers();
}

void MeshData::SetupBuffers() {
    vao.Bind(); 

    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    vao.LinkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    vao.LinkAttrib(vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
    vao.LinkAttrib(vbo, 3, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    vao.LinkAttrib(vbo, 4, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));

    ebo.Bind(); 

    vao.Unbind(); 
    vbo.Unbind();
}

void MeshData::Draw() {
    vao.Bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    vao.Unbind();
  
}