#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>
#include <optional>

class Texture3D
{
public:

    // Constructor with mipmaps and texture unit support
    Texture3D(GLenum internalFormat, GLenum format, GLenum type,
        int width, int height, int depth, std::optional<const void*> data = std::nullopt,
        GLenum slot = GL_TEXTURE0, bool generateMipmaps = false);


    Texture3D(Texture3D&& other) noexcept;
    Texture3D& operator=(Texture3D&& other) noexcept;
    ~Texture3D();

    void Bind() const;
    static void Unbind();

    void UploadData(const void* data, GLenum format, GLenum type,
        int xOffset, int yOffset, int zOffset, int subWidth, int subHeight, int subDepth);

    //Getters
    GLuint GetID() const { return id; }
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetDepth() const { return depth; }
    bool HasMipMaps() const { return hasMipMaps; }

private:

    GLuint id = 0;
    int width = 0;
    int height = 0;
    int depth = 0;
    bool hasMipMaps = false;
    GLenum slot;

};

