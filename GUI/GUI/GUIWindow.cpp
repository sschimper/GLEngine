#include <GUIStdafx.h>

#include <GUI/GUIWindow.h>

#include <Core/GUID.h>

namespace GLEngine::GUI {

//=================================================================================
C_Window::C_Window(GUID guid, const std::string& name)
	: m_GUID(guid)
	, m_Name(name)
	, m_IsVisible(false)
	, m_WantToBeDestroyed(false)
{
}

//=================================================================================
bool C_Window::Draw(C_GUIManager& guiMgr) const
{
	ImGuiWindowFlags flags = 0;
	if (!m_Menus.empty())
	{
		flags |= ImGuiWindowFlags_MenuBar;
	}
	::ImGui::Begin(m_Name.c_str(), &m_IsVisible, flags);
	DrawMenus();

	DrawComponents();
	::ImGui::End();
	return false; // todo!
}

//=================================================================================
void C_Window::DrawComponents() const
{
	for (const auto& component : m_Components)
	{
		component.second.get().Draw();
	}
}

//=================================================================================
void C_Window::SetVisible(bool enable /*= true*/)
{
	if (m_IsVisible != enable) {
		if (enable)
			OnSetVisible();
		else
			OnHide();
	}
	m_IsVisible = enable;
}

//=================================================================================
bool C_Window::IsVisible() const
{
	return m_IsVisible;
}

//=================================================================================
GUID C_Window::AddComponent(C_Window::T_GUIPartRef component)
{
	GUID guid = NextGUID();
	m_Components.emplace_back(guid, component);
	return guid;
}

//=================================================================================
I_GUIPart* C_Window::GetComponent(GUID guid) const
{
	const auto& it = std::find_if(m_Components.begin(), m_Components.end(), [&guid](const auto& pair) { return pair.first == guid; });
	if (it != m_Components.end())
	{
		return &(it->second.get());
	}
	return nullptr;
}

//=================================================================================
GUID C_Window::AddMenu(T_GUIMenu menuItem)
{
	GUID guid = NextGUID();
	m_Menus.emplace(guid, menuItem);
	return guid;
}

//=================================================================================
void C_Window::DrawMenus() const
{
	if (!m_Menus.empty())
	{
		if (::ImGui::BeginMenuBar())
		{
			for (const auto& menu : m_Menus)
			{
				menu.second.get().Draw();
			}
			::ImGui::EndMenuBar();
		}
	}
}

} // namespace GLEngine::GUI