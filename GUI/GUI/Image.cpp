#include <GUIStdafx.h>

#include <GUI/Image.h>

#include <imgui_internal.h>

namespace GLEngine::GUI {

//=================================================================================
C_Image::C_Image(Renderer::I_DeviceTexture& texture)
	: m_texture(texture)
	, m_Zoom(2.f)
{
	SetSize(m_texture.get().GetDimensions());
}

//=================================================================================
void C_Image::Draw() const
{
	// drawing area
	const auto io = ImGui::GetIO();
	m_Zoom		  = std::clamp(m_Zoom + io.MouseWheel / 10.f, 1.f, 10.f);

	const ImVec2 drawAreaSz(m_Size.x, m_Size.y);
	const ImVec2 canvas_p0 = ImGui::GetCursorPos();
	const ImRect imageRect(canvas_p0, canvas_p0 + drawAreaSz);
	ImGui::InvisibleButton("canvas", drawAreaSz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
	const bool is_hovered = ImGui::IsItemHovered(); // Hovered
	const bool is_active  = ImGui::IsItemActive();	// Held
	ImGui::SetCursorPos(canvas_p0);
	ImRect zoomArea({0, 0}, {1, 1});
	if (is_hovered)
	{
		// mouse pos is defined in screenspace
		const auto mousePos = ImGui::GetMousePos() - ImGui::GetCursorScreenPos();
		ImVec2	   mouseUV(mousePos.x / drawAreaSz.x, mousePos.y / drawAreaSz.y);
		mouseUV.x	 = glm::clamp(mouseUV.x, 1.f / m_Zoom/ 2.f, 1.f - 1.f / m_Zoom/ 2.f);
		mouseUV.y	 = glm::clamp(mouseUV.y, 1.f / m_Zoom/ 2.f, 1.f - 1.f / m_Zoom/ 2.f);
		zoomArea.Min = mouseUV - ImVec2{1.f / m_Zoom / 2.f, 1.f / m_Zoom / 2.f};
		zoomArea.Max = mouseUV + ImVec2{1.f / m_Zoom / 2.f, 1.f / m_Zoom / 2.f};
	}

	ImGui::Image((void*)(intptr_t)(m_texture.get().GetGPUHandle()), drawAreaSz, zoomArea.Min, zoomArea.Max);
}

//=================================================================================
void C_Image::SetSize(const glm::vec2 dim)
{
	m_Size = dim;
}

} // namespace GLEngine::GUI