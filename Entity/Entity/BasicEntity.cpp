#include <EntityStdafx.h>

#include <Entity/BasicEntity.h>
#include <Entity/Components/IDebugGUIComponent.h>
#include <Entity/IComponent.h>

#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<GLEngine::Entity::C_BasicEntity>("C_BasicEntity")
		.constructor<std::string>()(rttr::policy::ctor::as_std_shared_ptr)
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr);

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<GLEngine::Entity::C_BasicEntity>>();
}

namespace GLEngine::Entity {

//=================================================================================
C_BasicEntity::C_BasicEntity(std::string name)
	: I_Entity(std::move(name))
	, m_ModelMatrix(glm::mat4(1.f))
{
}

//=================================================================================
C_BasicEntity::C_BasicEntity()
	: I_Entity("")
	, m_ModelMatrix(glm::mat4(1.f))
{
}

//=================================================================================
C_BasicEntity::~C_BasicEntity()
{
	CORE_LOG(E_Level::Error, E_Context::Core, "~C_BasicEntity()");
}

//=================================================================================
void C_BasicEntity::Update()
{
	for (auto comp : *m_Components)
	{
		comp.second->Update();
	}
}

//=================================================================================
void C_BasicEntity::PostUpdate()
{
	for (auto& comp : *m_Components)
	{
		comp.second->PostUpdate();
	}
}

//=================================================================================
void C_BasicEntity::OnEvent(Core::I_Event& event)
{
	auto debugGUI = GetComponent<E_ComponentType::DebugGUI>();
	if (debugGUI)
	{
		debugGUI->Toggle();
	}
}

//=================================================================================
void C_BasicEntity::SetModelMatrix(const glm::mat4& modelMatrix)
{
	m_ModelMatrix = modelMatrix;
}

//=================================================================================
const glm::mat4& C_BasicEntity::GetModelMatrix() const
{
	return m_ModelMatrix;
}

//=================================================================================
glm::vec3 C_BasicEntity::GetPosition() const
{
	return m_ModelMatrix[3];
}

} // namespace GLEngine::Entity