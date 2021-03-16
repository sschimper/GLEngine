#pragma once

#include <Core/Application.h>
#include <Core/CoreMacros.h>
#include <Core/EventSystem/Layer.h>
#include <Core/WindowInfo.h>

namespace GLEngine {

namespace Renderer {
class I_Renderer;
}


namespace Core {
class I_Input;

/************************************************************************/
/* Represents single window on screen                                   */
/************************************************************************/
class I_Window : public C_Layer {
public:
	virtual ~I_Window()						 = default;
	virtual unsigned int   GetWidth() const	 = 0;
	virtual unsigned int   GetHeight() const = 0;
	virtual glm::uvec2	   GetSize() const	 = 0;
	virtual const I_Input& GetInput() const	 = 0;
	/**
	 * Transforms screen space coordinates to clip space
	 * Clip space have (0,0) in the middle of viewport
	 */
	virtual glm::vec2 ToClipSpace(const glm::vec2& screenCoord) const = 0;

	virtual void Update() = 0;

	/** ==============================================
	 * @method:    Init
	 * @fullName:  Core::I_Window::Init
	 * @return:    void
	 * @param: 	   const S_WindowInfo & - should be corresponding subcalss
	 * @brief
	 ** ==============================================*/
	virtual void Init(const S_WindowInfo& wndInfo)	= 0;
	virtual void SetTitle(const std::string& title) = 0;

	inline void SetEventCallback(C_Application::EventCallbackFn callback) { m_Data.m_EventCallback = callback; }

	[[nodiscard]] virtual bool WantClose() const = 0;
	[[nodiscard]] virtual bool CanClose() const { return true; }

	virtual const std::unique_ptr<GLEngine::Renderer::I_Renderer>& GetRenderer() const = 0;

	inline GUID GetGUID() const { return m_ID; }

protected:
	virtual void Destroy() = 0;
	I_Window()
		: C_Layer("window")
		, m_ID(NextGUID())
		, m_Data({nullptr, INVALID_GUID})
	{
	}
	GUID m_ID;


	struct S_Data {
		Core::C_Application::EventCallbackFn m_EventCallback;
		GUID								 m_GUID;
	};

	S_Data m_Data;
};
} // namespace Core
} // namespace GLEngine