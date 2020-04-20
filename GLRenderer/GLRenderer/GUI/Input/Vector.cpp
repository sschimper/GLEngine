#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/Input/Vector.h>

namespace GLEngine::GLRenderer::GUI::Input {

//=================================================================================
C_Vec3::C_Vec3(std::string&& name, glm::vec3 default) 
	: C_Vector(std::move(name), default) { }

//=================================================================================
void C_Vec3::Draw() const
{
	float* col;
	col = &(m_Value.x);
	::ImGui::InputFloat3(m_name.c_str(), col);
}

//=================================================================================
C_Vec3& C_Vec3::operator=(const glm::vec3& value)
{
	m_Value = value;
	return *this;
}

}