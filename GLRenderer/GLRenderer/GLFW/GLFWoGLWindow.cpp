#include <GLRendererStdafx.h>

#include <GLRenderer/GLFW/GLFWoGLWindow.h>

#include <GLRenderer/GLFW/OpenGLWindowInfo.h>
#include <Renderer/IRenderer.h>

#include <GLRenderer/OGLRenderer.h>

namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

//=================================================================================
C_GLFWoGLWindow::C_GLFWoGLWindow(const Core::S_WindowInfo& wndInfo)
	: m_renderer(nullptr)
{
	Init(wndInfo);
}

//=================================================================================
void C_GLFWoGLWindow::Update()
{
	glfwMakeContextCurrent(m_Window);
	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

//=================================================================================
void C_GLFWoGLWindow::Init(const Core::S_WindowInfo& wndInfo)
{
	// TODO: add assert here
	const auto wndInfoOGL = dynamic_cast<const S_OpenGLWindowInfo*>(&wndInfo);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, wndInfoOGL->m_MajorVersion);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, wndInfoOGL->m_MinorVersion);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	C_GLFWWindow::Init(wndInfo);
	glfwMakeContextCurrent(m_Window);

	m_renderer = std::make_unique<GLEngine::GLRenderer::C_OGLRenderer>();

	CORE_LOG(E_Level::Info, E_Context::Render, "GLFW: OpenGL window initialized");
}

}
}
}