#include <GLRendererStdafx.h>

#include <GLRenderer/Windows/ExperimentWindow.h>

#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GlClearColor.h>

#include <GLFW/glfw3.h>
#include <stdlib.h>

namespace GLEngine {
namespace GLRenderer {
namespace Windows {

//=================================================================================
C_ExplerimentWindow::C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo)
	: C_GLFWoGLWindow(wndInfo)
{

}

//=================================================================================
void C_ExplerimentWindow::Update()
{
	auto red = rand() % 255;
	Commands::C_GLClear clear(Commands::C_GLClear::E_ClearBits::Color | Commands::C_GLClear::E_ClearBits::Stencil);
	Commands::C_GLClearColor clearColor(glm::vec3(static_cast<float>(red) / 255.0f, 0.0f, 0.0f));
	glfwMakeContextCurrent(m_Window);

	
	clearColor.Commit();
	clear.Commit();

	glfwSwapBuffers(m_Window);
	glfwPollEvents();
}

}
}
}