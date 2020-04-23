#pragma once

#include <Renderer/Mesh/Scene.h>

namespace GLEngine::GLRenderer::Textures {
class TextureLoader
{
public:
	[[nodiscard]] bool loadTexture(const char* path, Renderer::MeshData::Texture& t);

private:
	static bool _isILinitialized;
};
}