#pragma once
#include "Renderer.h"
#include "ComputeBuffer.h"
#include "ComputeShader.h"
#include "ComputeImage.h"
#include <memory>
#include "RayTracingStructs.h"

class PathTraceRenderer : public Renderer
{

public:

	PathTraceRenderer(int width, int height, Shader& shader);
	~PathTraceRenderer();

	void Initialize(Scene& scene) override;
	void Render(Scene& scene, Camera& camera, float deltaTime) override;

private:

	// TODO : Scene Dirty fetch + Scene lazy update logic 

	GLuint m_quadVBO, m_quadVAO;
	Shader m_displayShader;

	//===== COMPUTE SYSTEM

	ComputeShader m_pathTraceShader;
	ComputeImage m_outputImage;

	ComputeBuffer<MeshInstanceCommand> m_meshInstanceBuffer;
	ComputeBuffer<MeshInfo> m_meshInfoBuffer;
	ComputeBuffer<Vertex> m_vertexBuffer;
	ComputeBuffer<unsigned int> m_indicesBuffer;

	//======= PATHTRACER SETTINGS

	int m_frameCounter = 0;
	int m_samplesPerPixel = 4;
	int m_maxRayBounces = 8;

private:

	void SetupDisplayQuad();
	void PrepareData(Scene& scene);
	void DispatchComputeShader();
	void DisplayResult();

	//===== PATHTRACER SETTINGS GETTERS&SETTERS

	const int& GetSamplesPerPixel() { return m_samplesPerPixel; }
	void ResetAccumulation() { m_frameCounter = 0; }
	void SetSamplesPerPixels(int samples) { m_samplesPerPixel = samples; ResetAccumulation(); }
	void SetMaxRayBounces(int rayBounces) { m_maxRayBounces = rayBounces; ResetAccumulation(); }

};

