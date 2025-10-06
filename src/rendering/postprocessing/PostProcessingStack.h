#pragma once
#include <memory>
#include "IPostProcessEffect.h"
#include <vector>
#include <unordered_map>
#include "FBO.h"
class PostProcessingStack
{
public:
	struct Settings {
		int width;
		int height;
		bool useHDR = true;
		unsigned int msaaSamples = 1;
	};

	PostProcessingStack(const Settings& settings);
	~PostProcessingStack();

	void AddEffect(std::unique_ptr<IPostProcessEffect> effect);
	void ClearEffects();
	void RemoveEffect(const std::string& name);
	IPostProcessEffect* GetEffect(const std::string& name);

	void BeginRender();
	void EndRender();
	void ApplyEffects();


private:

	Settings settings;

	std::unique_ptr<IFramebuffer> msaaFBO;
	std::unique_ptr<FBO> intermediateFBOs[2];
	std::unique_ptr<FBO> outputFBO;

	std::vector<std::unique_ptr<IPostProcessEffect>> effects;
	std::unordered_map<std::string, IPostProcessEffect*> effectMap;

	bool needsDepth = false;
	bool frameStarted = false;

	void CreateFramebuffers();
	void DestroyFramebuffers();
};

