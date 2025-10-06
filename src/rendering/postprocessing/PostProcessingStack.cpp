#include "PostProcessingStack.h"
#include "FramebufferFactory.h"
#include "MSAAFBO.h"

PostProcessingStack::PostProcessingStack(const Settings& settings) :
	settings(settings)
{
	CreateFramebuffers();
}

PostProcessingStack::~PostProcessingStack()
{
	DestroyFramebuffers();
}

void PostProcessingStack::AddEffect(std::unique_ptr<IPostProcessEffect> effect)
{
	if (!effect) return;

	const std::string name = effect->GetName();
	if (effectMap.find(name) != effectMap.end()) {
		std::cout << "effect " + name + " already exists on the stack";
		return;
	}

	// Capture all needed info BEFORE moving
	IPostProcessEffect* rawPtr = effect.get();
	bool requiresDepth = effect->RequiresDepthTexture();

	// Transfer ownership
	effects.push_back(std::move(effect));

	// Use captured values after move
	effectMap[name] = rawPtr;
	if (requiresDepth) {  // Safe: uses pre-captured value
		needsDepth = true;
	}

}

void PostProcessingStack::ClearEffects()
{
	effects.clear();
	effectMap.clear();
	needsDepth = false;
}

void PostProcessingStack::RemoveEffect(const std::string& name)
{
	auto it = effectMap.find(name);
	if (it == effectMap.end()) return;

	effects.erase(
		std::remove_if(effects.begin(), effects.end(),
			[&](const std::unique_ptr<IPostProcessEffect>& effect) {
				return effect.get() == it->second;
			}), effects.end());
	effectMap.erase(it);
}

IPostProcessEffect* PostProcessingStack::GetEffect(const std::string& name)
{
	auto it = effectMap.find(name);
	return it != effectMap.end() ? it->second : nullptr;
}

void PostProcessingStack::BeginRender()
{
	
	if (msaaFBO) {
		glEnable(GL_MULTISAMPLE);
		msaaFBO->Bind();
	}
	else {
		intermediateFBOs[0]->Bind();
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	frameStarted = true;
}

void PostProcessingStack::EndRender()
{
	if (!frameStarted) return;

	if (msaaFBO) {
		auto msaa = dynamic_cast<MSAAFBO*>(msaaFBO.get());
		if (msaa) {
			intermediateFBOs[0]->Bind();
			msaa->ResolveToTexture(intermediateFBOs[0]->GetID());
			intermediateFBOs[0]->Unbind();
		}
		glDisable(GL_MULTISAMPLE);
	}
	frameStarted = false;
}

void PostProcessingStack::ApplyEffects()
{
	if (effects.empty()) return;

	glActiveTexture(GL_TEXTURE0);
	GLuint currentTexture = intermediateFBOs[0]->GetColorAttachment().ID;
	glBindTexture(GL_TEXTURE_2D, currentTexture);


	if (msaaFBO && effects.size() == 1) {
		outputFBO->Bind();
		effects[0]->Apply( currentTexture, true);
		outputFBO->Unbind();
		return;
	}

	bool useFirstIntermediate = true;
	for (size_t i = 0; i < effects.size(); ++i) {
		const bool lastEffect = (i == effects.size() - 1);
		FBO& targetFBO = lastEffect ? *outputFBO :
			*(useFirstIntermediate ? intermediateFBOs[1] : intermediateFBOs[0]);

		targetFBO.Bind();
		effects[i]->Apply( currentTexture, lastEffect);
		targetFBO.Unbind();

		if (!lastEffect) {
			currentTexture = targetFBO.GetColorAttachment().ID;
			useFirstIntermediate = !useFirstIntermediate;
		}
	}
	
}

void PostProcessingStack::CreateFramebuffers()
{
	if (settings.msaaSamples > 1) {
		msaaFBO = FramebufferFactory::Create(
			settings.width, settings.height,
			FramebufferType::DEPTH_STENCIL,
			false, true, false, 1,
			FramebufferFactory::Type::MSAA,
			settings.msaaSamples
		);
	}

	for (auto& framebuffer : intermediateFBOs) {
		framebuffer = std::make_unique<FBO>(
			settings.width, settings.height,
			FramebufferType::DEPTH_STENCIL,
			false,std::nullopt, true, false, 1
		);
	}

	outputFBO = std::make_unique<FBO>(
		settings.width, settings.height,
		FramebufferType::DEPTH_STENCIL,
		false,std::nullopt, true, false, 1
	);

}

void PostProcessingStack::DestroyFramebuffers()
{
	msaaFBO.reset();
	intermediateFBOs[0].reset();
	intermediateFBOs[1].reset();
	outputFBO.reset();
}
