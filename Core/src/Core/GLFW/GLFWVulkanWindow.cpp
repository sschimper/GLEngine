#include <CoreStdafx.h>

#include <Core/GLFW/GLFWVulkanWindow.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Core {
namespace GLFW {
//=================================================================================
C_GLFWVulkanWindow::C_GLFWVulkanWindow()
{
	Init();
}

//=================================================================================
void C_GLFWVulkanWindow::Update()
{

}

//=================================================================================
void C_GLFWVulkanWindow::Init()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	C_GLFWWindow::Init();

}

}
}
