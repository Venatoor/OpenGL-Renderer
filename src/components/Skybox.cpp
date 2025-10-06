#include "Skybox.h"

Skybox::Skybox(const std::vector<std::string>& cubemapFaces, Shader& shader) :

	skyboxCubemap(cubemapFaces),
	skyboxShader(shader)

{
	SetupSkyboxGeometry();
	skyboxShader.Activate();
	skyboxShader.SetInt("skybox", 0);
}

Skybox::~Skybox()
{
}


void Skybox::Draw(const glm::mat4& view, const glm::mat4& projection)
{
    glDepthFunc(GL_LEQUAL);

    skyboxShader.Activate();

    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view));
    skyboxShader.SetMat4("view", viewNoTranslation);
    skyboxShader.SetMat4("projection", projection);

    skyboxCubemap.Bind(GL_TEXTURE0);

    vao.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    vao.Unbind();

    glDepthFunc(GL_LESS);
}

void Skybox::SetupSkyboxGeometry()
{
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    vbo = VBO(skyboxVertices, sizeof(skyboxVertices));
    
    vao.Bind();
    vbo.Bind();
    
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    vbo.Unbind();
    vao.Unbind();
    
}

const int& Skybox::GetCubeMapID() {
    return skyboxCubemap.GetID();
}