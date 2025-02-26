#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UniformBuffersManager.h>

#include <GLRenderer/Buffers/UniformBuffer.h>

#include <GLRenderer/Shaders/ShaderProgram.h>

#include <algorithm>

namespace GLEngine {
namespace GLRenderer {
namespace Buffers {

//=================================================================================
C_UniformBuffersManager::C_UniformBuffersManager()
{

}

//=================================================================================
C_UniformBuffersManager& C_UniformBuffersManager::Instance()
{
	static C_UniformBuffersManager    instance; // Guaranteed to be destroyed.
										// Instantiated on first use.
	return instance;
}

//=================================================================================
void C_UniformBuffersManager::PrintStatistics() const
{

}

//=================================================================================
void C_UniformBuffersManager::Clear()
{
	m_UBOs.clear();
}

//=================================================================================
void C_UniformBuffersManager::BindUBOs(const Shaders::C_ShaderProgram* program) const
{
	GLE_ASSERT(program, "Invalid shader program given");
	for (const auto& ubo : m_UBOs) {
		if (ubo->IsActive()) {
			program->BindUBO(ubo);
		}
	}
}

//=================================================================================
C_UniformBuffersManager::T_UBOSmartPtr C_UniformBuffersManager::GetBufferByName(const std::string& name) const
{
	return *std::find_if(m_UBOs.begin(), m_UBOs.end(), [name](const T_UBOSmartPtr& ubo) {
		return name == ubo->GetBlockName();
	});
}

//=================================================================================
void C_UniformBuffersManager::ProcessUBOBindingPoints(std::shared_ptr<Shaders::C_ShaderProgram> program) const
{
	int i = 0;
	for (const auto & ubo : m_UBOs)
	{
		if (!ubo) {
			continue;
		}
		auto uniformBlockIndex = program->FindUniformBlockLocation<const std::string&>(ubo->GetBlockName());
		if (uniformBlockIndex != GL_INVALID_INDEX) {
			glUniformBlockBinding(program->GetProgram(), uniformBlockIndex, i);
		}
		++i;
	}
}

}}}