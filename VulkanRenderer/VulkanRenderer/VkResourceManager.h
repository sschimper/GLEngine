#pragma once

#include <VulkanRenderer/VkBuffer.h>

#include <Renderer/Resources/ResourceManager.h>

namespace GLEngine::VkRenderer {
class C_VkDevice;

class C_VkResourceManager : public Renderer::ResouceManager {
public:
	C_VkResourceManager();
	~C_VkResourceManager() noexcept = default;

	void Init(C_VkDevice* device);

	[[nodiscard]] Renderer::Handle<Renderer::Shader>  createShader(const std::filesystem::path& path) override;
	void											  destoryShader(Renderer::Handle<Renderer::Shader> handle) override;
	[[nodiscard]] Renderer::Handle<Renderer::Texture> createTexture(const Renderer::TextureDescriptor& desc) override;
	void											  destoryTexture(Renderer::Handle<Renderer::Texture> handle) override;
	[[nodiscard]] Renderer::Handle<Renderer::Buffer>  createBuffer(const Renderer::BufferDescriptor& desc) override;
	void											  destroyBuffer(const Renderer::Handle<Renderer::Buffer>& handle) override;

	C_VkBuffer* GetBuffer(const Renderer::Handle<Renderer::Buffer>& handle);

private:
	Renderer::ResourcePool<Renderer::Buffer, C_VkBuffer> m_BufferPool;
	C_VkDevice*											 m_device;
};
} // namespace GLEngine::VkRenderer
