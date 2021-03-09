#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Mesh/Scene.h>
#include <Renderer/Textures/TextureStorage.h>

namespace GLEngine::Renderer::Textures {
class RENDERER_API_EXPORT TextureLoader
{
public:
	[[nodiscard]] bool loadTexture(const std::filesystem::path& path, MeshData::Texture& t);
	[[nodiscard]] I_TextureViewStorage* loadTexture(const std::filesystem::path& path);

private:
	static bool _isILinitialized;
};
}