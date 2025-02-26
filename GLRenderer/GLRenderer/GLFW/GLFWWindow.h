#pragma once

#include <Core/IWindow.h>

#include <Core/Application.h>
#include <GLRenderer/GLFW/GLFWInput.h>

struct GLFWwindow;

namespace GLEngine {
namespace Core {
class I_Input;
}

namespace GLRenderer {
namespace GLFW {
class C_GLFWWindow : public Core::I_Window {
public:
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

	virtual void Init(const Core::S_WindowInfo& wndInfo) override;


	//=================================================================================
	virtual void SetEventCallback(Core::C_Application::EventCallbackFn callback) override;


	//=================================================================================
	virtual const Core::I_Input& GetInput() const override;

protected:
	C_GLFWWindow();
	virtual void Destroy() override;

	struct S_Data {
		Core::C_Application::EventCallbackFn m_EventCallback;
		GUID m_GUID;
	};

	S_Data m_Data;

	GLFWwindow* m_Window;
	C_GLFWInput m_Input;
};
}
}
}