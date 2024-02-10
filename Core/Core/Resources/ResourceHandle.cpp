#include <CoreStdafx.h>

#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Reflection/Metadata.h>

#include <rttr/registration>

RTTR_REGISTRATION
{
	using namespace GLEngine::Core;
	using namespace Utils::Reflection;

	rttr::registration::class_<ResourceHandleBase>("ResourceHandleBase")
		.constructor<std::shared_ptr<Resource>>()()
		.constructor<>()()
		.property("Resource", &ResourceHandleBase::m_Resource);
}

namespace GLEngine::Core {

//=================================================================================
ResourceHandleBase::ResourceHandleBase(std::shared_ptr<Resource> resource)
	: m_Resource(resource)
{
}

//=================================================================================
ResourceHandleBase::ResourceHandleBase()
	: m_Resource(nullptr)
{
}

//=================================================================================
ResourceState ResourceHandleBase::GetState() const
{
	if (m_Resource)
		return m_Resource->GetState();
	return ResourceState::Failed;
}

//=================================================================================
ResourceHandleBase::~ResourceHandleBase()
{
	if (m_Resource.use_count() == 2)
	{
		C_ResourceManager::Instance().AddResourceToUnusedList(m_Resource);
	}
}

//=================================================================================
bool ResourceHandleBase::IsReady() const
{
	return GetState() == ResourceState::Ready;
}

//=================================================================================
bool ResourceHandleBase::IsLoading() const
{
	return GetState() == ResourceState::Loading;
}

//=================================================================================
bool ResourceHandleBase::IsFailed() const
{
	return GetState() == ResourceState::Failed;
}

//=================================================================================
const std::filesystem::path& ResourceHandleBase::GetFilePath() const
{
	if (m_Resource)
		return m_Resource->GetFilePath();
	return "";
}
} // namespace GLEngine::Core
