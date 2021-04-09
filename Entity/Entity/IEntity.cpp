#include <EntityStdafx.h>

#include <Entity/ComponentManager.h>
#include <Entity/IEntity.h>

#include <Physics/Primitives/AABB.h>

#include <Utils/MapValueIterator.h>

namespace GLEngine::Entity {

//=================================================================================
I_Entity::I_Entity(std::string name)
	: m_Name(std::move(name))
	, m_Components(new std::remove_pointer<decltype(m_Components)>::type)
	, m_ID(NextGUID())
{
	CORE_LOG(E_Level::Info, E_Context::Entity, "Entity '{}' created.", m_Name);
}

//=================================================================================
I_Entity::~I_Entity()
{
	auto& compManager = Entity::C_ComponentManager::Instance();
	std::for_each(m_Components->begin(), m_Components->end(), [&compManager](const auto& component) { compManager.UnregisterComonent(component.second); });
	delete m_Components;

	CORE_LOG(E_Level::Info, E_Context::Entity, "Entity '{}' despawn", m_Name);
}

//=================================================================================
I_Entity::T_ComponentRange I_Entity::GetComponents(E_ComponentType type) const
{
	auto iterLow = m_Components->equal_range(type);
	return {Utils::MapValueIterator(std::move(iterLow.first)), Utils::MapValueIterator(std::move(iterLow.second))};
}

//=================================================================================
Physics::Primitives::S_AABB I_Entity::GetAABB() const
{
	Physics::Primitives::S_AABB aabb;
	std::for_each(m_Components->begin(), m_Components->end(), [&aabb](const auto& component) {
		auto componentAABB = component.second->GetAABB();
		aabb.Add(componentAABB.getTransformedAABB(component.second->GetComponentModelMatrix()));
	});

	return aabb;
}

//=================================================================================
void I_Entity::AddComponent(T_ComponentPtr component)
{
	Entity::C_ComponentManager::Instance().RegisterComponent(component);
	m_Components->insert({component->GetType(), component});
}


//=================================================================================
I_Entity::T_ComponentIter I_Entity::begin()
{
	return m_Components->begin();
}

//=================================================================================
I_Entity::T_ComponentIter I_Entity::end()
{
	return m_Components->end();
}

} // namespace GLEngine::Entity