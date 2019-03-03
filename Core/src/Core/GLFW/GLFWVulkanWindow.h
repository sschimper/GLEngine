#pragma once

#include <Core/GLFW/GLFWWindow.h>

namespace Core {
namespace GLFW {

class API_EXPORT C_GLFWVulkanWindow : public C_GLFWWindow {
public:
	C_GLFWVulkanWindow();
	virtual ~C_GLFWVulkanWindow() = default;
	virtual void Update() override;

protected:
	virtual void Init() override;
private:
	float m_color = 0.0f;
};
}
}

