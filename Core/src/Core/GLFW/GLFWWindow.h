#pragma once

#include <Core/IWindow.h>

struct GLFWwindow;

namespace Core {
namespace GLFW {
class API_EXPORT C_GLFWWindow : public I_Window {
public:
	C_GLFWWindow();
	virtual ~C_GLFWWindow();


	//============================================================
	// I_Window
	//============================================================
	virtual unsigned int GetWidth() const override;
	virtual unsigned int GetHeight() const override;
	virtual glm::uvec2 GetSize() const override;
	virtual void SetTitle(const std::string& title) override;
	virtual void Update() override;
	virtual bool WantClose() const override;

protected:
	virtual void Init();
	virtual void Destroy();


	GLFWwindow* m_Window;
};
}
}