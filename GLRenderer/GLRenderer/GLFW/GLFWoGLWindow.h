#pragma once

#include <GLRenderer/GLFW/GLFWWindow.h>

#include <Renderer/IRenderer.h>

namespace GLEngine {

namespace Core {
class C_KeyPressedEvent;
}

namespace GLRenderer::GLFW {

class C_GLFWoGLWindow : public C_GLFWWindow {
public:
	explicit C_GLFWoGLWindow(const Core::S_WindowInfo& wndInfo);
	virtual ~C_GLFWoGLWindow() = default;
	virtual void Update() override;

	[[nodiscard]] virtual const std::unique_ptr<Renderer::I_Renderer>& GetRenderer() const override;
	virtual void													   OnEvent(Core::I_Event& event) override;

protected:
	virtual void						  Init(const Core::S_WindowInfo& wndInfo) override;
	std::unique_ptr<Renderer::I_Renderer> m_renderer;
};

} // namespace GLRenderer::GLFW
} // namespace GLEngine
