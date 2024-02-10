#pragma once

#include <Renderer/RendererApi.h>

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceLoader.h>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT AnimationResource : public Core::Resource {
public:
	DEFINE_RESOURCE_TYPE(AnimationResource)

	virtual bool Load(const std::filesystem::path& filepath) override;
	virtual bool Reload() override;
};

class RENDERER_API_EXPORT AnimationLoader : public Core::ResourceLoader<AnimationResource> {
public:
	virtual std::shared_ptr<Core::Resource> CreateResource() const override;
	virtual std::vector<std::string>		GetSupportedExtensions() const override;
};
} // namespace GLEngine::Renderer
