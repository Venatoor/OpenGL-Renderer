#include "ShadowMap.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "FramebufferFactory.h"

ShadowMap::ShadowMap(int width, int height) :
	width(width),
	height(height),
	lightViewProj(1.0f)
{
	framebuffer = FramebufferFactory::Create(width, height, FramebufferType::DEPTH_STENCIL, false, true, true, 0
	,FramebufferFactory::Type::SM, 0);
}

ShadowMap::~ShadowMap()
{
	framebuffer.reset(); 
}

SMFBO* ShadowMap::GetSMFBO() const {
	SMFBO* smfbo = dynamic_cast<SMFBO*>(framebuffer.get());
	if (!smfbo) {
		throw std::runtime_error("Framebuffer is not of type SMFBO");
	}
	return smfbo;
}

/*
void ShadowMap::UpdateFromDirectionalLight(const DirectionalLight& light, const glm::vec3& focusPoint, float coverage)
{
	if (!light.enabled) return;

	glm::vec3 lightDirNorm = glm::normalize(light.direction);
	float distance = coverage;


	const glm::vec3 lightPos = focusPoint - lightDirNorm * distance;

	std::cout << "Directional Light Info:\n";
	std::cout << " - light.direction (raw): (" << light.direction.x << ", " << light.direction.y << ", " << light.direction.z << ")\n";
	std::cout << " - light.direction (normalized): (" << lightDirNorm.x << ", " << lightDirNorm.y << ", " << lightDirNorm.z << ")\n";
	std::cout << " - coverage: " << coverage << "\n";
	std::cout << " - distance (coverage*20): " << distance << "\n";
	std::cout << " - focusPoint: (" << focusPoint.x << ", " << focusPoint.y << ", " << focusPoint.z << ")\n";
	std::cout << " - computed lightPos: (" << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << ")\n";

	glm::vec3 up = (abs(lightDirNorm.y) > 0.9f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
	glm::mat4 lightView = glm::lookAt(lightPos, focusPoint, up);


	const float halfSize = coverage;
	glm::mat4 lightProj = glm::ortho(-halfSize, halfSize,
		-halfSize, halfSize,
		0.1f, distance * 2.0f);

	lightViewProj = lightProj * lightView;

	std::cout << "\nFinal LightViewProj Matrix:\n";
	for (int i = 0; i < 4; ++i) {
		printf("%8.3f %8.3f %8.3f %8.3f\n",
			lightViewProj[i][0], lightViewProj[i][1],
			lightViewProj[i][2], lightViewProj[i][3]);
	}

	// 5. Validation checks
	std::cout << "\nMatrix Validation:\n";
	float det = glm::determinant(lightViewProj);
	std::cout << "Determinant: " << det << (abs(det) < 1e-6 ? " (WARNING: Degenerate!)" : "") << "\n";

	glm::vec4 testPoint = lightViewProj * glm::vec4(focusPoint, 1.0);
	testPoint /= testPoint.w;
}
*/

void ShadowMap::UpdateFromSpotLight(const SpotLight& light) {

	if (!light.enabled) return;

	// Normalize light direction
	glm::vec3 lightDir = glm::normalize(light.direction);

	// Calculate light projection matrix (perspective)
	// Convert cutoff angle to FOV (multiply by 2 for full cone angle)
	float fovRad = 2.0f * glm::acos(glm::clamp(light.outerCutOff, 0.01f, 0.99f));
	float fovDeg = glm::degrees(fovRad);
	//std::cout << "Valid FOV: " << fovDeg << " degrees\n";

	float aspectRatio = 1.0f; // Square shadow map
	float nearPlane = 0.1f;
	float farPlane = 100.f;

	/*
	glm::mat4 lightProj = glm::perspective(
		fovRad, 1.0f, 0.1f, 100.f
	);

	// Calculate light view matrix - similar to your Camera's lookAt
	// 4. ROBUST UP-VECTOR
	glm::vec3 up = glm::abs(lightDir.y) > 0.999f ? glm::vec3(0, 0, 1)
		: glm::vec3(0, 1, 0);

	glm::mat4 lightView = glm::lookAt(light.position,
		light.position + lightDir,
		up);
	*/

	glm::mat4 lightView = glm::lookAt(light.position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 lightProj = glm::perspective(glm::radians(90.f),1.0f,0.1f,100.f);

	// Combine matrices (same order as your Camera class)
	lightViewProj = lightProj * lightView;
	
	/*
	std::cout << "\nSpot Light Shadow Matrix:\n";
	std::cout << "Position: (" << light.position.x << ", "
		<< light.position.y << ", "
		<< light.position.z << ")\n";
	std::cout << "Direction: (" << lightDir.x << ", "
		<< lightDir.y << ", "
		<< lightDir.z << ")\n";
	std::cout << "FOV: " << fovDeg << " degrees\n";
	std::cout << "Radius (far plane): " << farPlane << "\n";

	// Print view matrix
	std::cout << "Light View Matrix:\n";
	for (int i = 0; i < 4; ++i) {
		printf("%8.3f %8.3f %8.3f %8.3f\n",
			lightView[i][0], lightView[i][1],
			lightView[i][2], lightView[i][3]);
	}

	// Print projection matrix
	std::cout << "\nLight Projection Matrix:\n";
	for (int i = 0; i < 4; ++i) {
		printf("%8.3f %8.3f %8.3f %8.3f\n",
			lightProj[i][0], lightProj[i][1],
			lightProj[i][2], lightProj[i][3]);
	}

	// Print combined matrix
	std::cout << "\nLight ViewProj Matrix:\n";
	for (int i = 0; i < 4; ++i) {
		printf("%8.3f %8.3f %8.3f %8.3f\n",
			lightViewProj[i][0], lightViewProj[i][1],
			lightViewProj[i][2], lightViewProj[i][3]);
	}

	// Validation checks
	std::cout << "\nValidation:\n";
	float det = glm::determinant(lightViewProj);
	std::cout << "Determinant: " << det << (abs(det) < 1e-6 ? " (WARNING: Near-zero!)" : "") << "\n";

	// Test point transformation
	glm::vec4 testPoint = lightViewProj * glm::vec4(light.position, 1.0);
	std::cout << "Light position in clip space: ("
		<< testPoint.x << ", " << testPoint.y << ", " << testPoint.z << ")\n";
	testPoint /= testPoint.w;
	std::cout << "Light position in clip space: ("
		<< testPoint.x << ", " << testPoint.y << ", " << testPoint.z << ")\n";
	*/
}


void ShadowMap::BeginShadowPass()
{
	GetSMFBO()->Bind(); 
	glViewport(0, 0, width, height);  
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_FRONT);  

}

void ShadowMap::EndShadowPass()
{
	glCullFace(GL_BACK);
	GetSMFBO()->Unbind();
}

void ShadowMap::BindTexture(GLenum textureUnit) const
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, GetSMFBO()->GetDepthTexture());
}


