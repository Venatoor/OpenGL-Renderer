#include "PathTraceRenderer.h"

PathTraceRenderer::PathTraceRenderer(int width, int height, Shader& shader) :
	m_pathTraceShader("raytracer.comp"),
	m_outputImage(width, height, GL_RGBA32F, GL_TEXTURE_2D),
	m_meshInstanceBuffer(0, GL_DYNAMIC_DRAW),
	m_indicesBuffer(0, GL_DYNAMIC_DRAW),
	m_meshInfoBuffer(0, GL_DYNAMIC_DRAW),
	m_vertexBuffer(0, GL_DYNAMIC_DRAW),
	m_displayShader(shader)

{
	SetupDisplayQuad();
	std::cout << "Created a quad with caracteristics : " << width << " " << height << std::endl;
}

PathTraceRenderer::~PathTraceRenderer()
{
	if (m_quadVAO) { glDeleteVertexArrays(1, &m_quadVAO); }
	if (m_quadVBO) { glDeleteBuffers(1, &m_quadVBO); }
	std::cout << "Pathtracer destroyed !" << std::endl;
}

void PathTraceRenderer::Initialize(Scene& scene)
{
	PrepareData(scene);
	std::cout << "PathTracerRenderer initialised" << std::endl;
}

void PathTraceRenderer::Render(Scene& scene, Camera& camera, float deltaTime)
{
	m_frameCounter++;

	//checking if we need to do lazy data preparation mandatory !
	PrepareData(scene);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DispatchComputeShader();
	DisplayResult();
}

void PathTraceRenderer::SetupDisplayQuad()
{
	float quadVertices[] = {
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &m_quadVBO);

	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

void PathTraceRenderer::PrepareData(Scene& scene)
{
}

void PathTraceRenderer::DispatchComputeShader()
{

}

void PathTraceRenderer::DisplayResult()
{
	m_displayShader.Activate();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_outputImage.GetID());
	m_displayShader.SetInt("sceneTexture", 0);

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
