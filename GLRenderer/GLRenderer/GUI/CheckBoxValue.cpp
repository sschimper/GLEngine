#include <GLRendererStdafx.h>

#include <GLRenderer/GUI/CheckBoxValue.h>



namespace GLEngine {
namespace GLRenderer {
namespace GUI {
//=================================================================================
C_CheckBoxValue::C_CheckBoxValue(bool value, std::string&& name)
	: m_Value(value)
	, m_Name(std::move(name))
{

}

//=================================================================================
void C_CheckBoxValue::Draw() const
{
	::ImGui::Checkbox(m_Name.c_str(), &m_Value);
}

//=================================================================================
void C_CheckBoxValue::SetName(std::string&& name)
{
	m_Name = std::move(name);
}

//=================================================================================
C_CheckBoxValue::operator bool() const
{
	return m_Value;
}

//=================================================================================
bool C_CheckBoxValue::operator==(bool val) const
{
	return m_Value == val;
}

//=================================================================================
bool C_CheckBoxValue::operator!=(bool val) const
{
	return !(*this == val);
}

//=================================================================================
C_CheckBoxValue C_CheckBoxValue::operator!()
{	
	return C_CheckBoxValue(!m_Value, std::string(m_Name));
}

//=================================================================================
bool* C_CheckBoxValue::operator&() const
{
	return &m_Value;
}

//=================================================================================
bool C_CheckBoxValue::operator=(bool val)
{
	return m_Value = val;
}

}}}