#include <GLRendererStdafx.h>

#include <GLRenderer/GLFW/GLFWInput.h>

#include <GLRenderer/GLFW/GLFWWindow.h>

namespace GLEngine {
namespace GLRenderer {
namespace GLFW {

//=================================================================================
C_GLFWInput::C_GLFWInput()
{

}

//=================================================================================
bool C_GLFWInput::IsKeyPressed(int keycode) const
{
	auto state = glfwGetKey(m_Window, keycode);
	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

//=================================================================================
bool C_GLFWInput::IsMosueButtonPresse(int key) const
{
	auto state = glfwGetMouseButton(m_Window, key);
	return state == GLFW_PRESS;
}

//=================================================================================
std::pair<float, float> C_GLFWInput::GetMousePosition() const
{
	std::pair<double, double> pos;
	glfwGetCursorPos(m_Window, &pos.first, &pos.second);

	return static_cast<std::pair<float, float>>(pos);
}

//=================================================================================
float C_GLFWInput::GetMouseX() const
{
	return static_cast<float>(GetMousePosition().first);
}

//=================================================================================
float C_GLFWInput::GetMouseY() const
{
	return static_cast<float>(GetMousePosition().second);
}

}}}