#include <Utils/Logging/LoggingMacros.h>
#include <Utils/Serialization/GLMRTTI.h>
#include <Utils/Serialization/XMLDeserialize.h>
#include <Utils/Serialization/SerializationTraits.h>

#include <rttr/type>

namespace GLEngine::Utils {

//=================================================================================
rttr::variant C_XMLDeserializer::DeserializeDoc(const pugi::xml_document& document)
{
	const auto rootNode = document.root().first_child();
	const auto type		= rttr::type::get_by_name(rootNode.name());
	if (type.is_valid() == false)
		return {};
	auto var = type.create();
	if (var.is_valid() == false)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Invalid variant created. Probably needs ctor to be registered.");
		return {};
	}
	return DeserializeNode(rootNode, var);
}

//=================================================================================
rttr::variant C_XMLDeserializer::DeserializeNode(const pugi::xml_node& node, rttr::variant& var)
{
	const rttr::instance var2 = var.get_type().get_raw_type().is_wrapper() ? rttr::instance(var).get_wrapped_instance() : var;
	//CORE_LOG(E_Level::Error, E_Context::Core, "{}", var2.get_type());

	for (auto& prop : var2.get_type().get_properties())
	{
		DeserializeProperty(prop, var, node);
	}

	return var;
}

//=================================================================================
void C_XMLDeserializer::DeserializeProperty(const rttr::property& prop, rttr::variant& owner, const pugi::xml_node& node)
{
	const auto type = prop.get_type().get_raw_type();
	if (IsAtomicType(type))
	{
		prop.set_value(owner, DeserializeAtomic(node.attribute(prop.get_name().to_string().c_str()), prop.get_type()));
	}
	else if (type.is_sequential_container())
	{
		const auto child = node.child(prop.get_name().to_string().c_str());
		auto	   var	 = prop.get_value(owner);
		auto	   view	 = var.create_sequential_view();
		DeserializeArray(child, view);
	}
	else if (type.is_associative_container())
	{
		const auto child = node.child(prop.get_name().to_string().c_str());
		auto	   var	 = prop.get_value(owner);
		auto	   view	 = var.create_associative_view();
		DeserializeAssociativeArray(child, view);
	}
	else
	{
		const auto child = node.child(prop.get_name().to_string().c_str());
		if (child)
			prop.set_value(owner, DeserializeNode(child, prop.get_value(owner)));
	}
}

//=================================================================================
void C_XMLDeserializer::DeserializeArray(const pugi::xml_node& child, rttr::variant_sequential_view& view)
{
	int index = 0;
	for (const auto childNode : child) {
		const auto type = rttr::type::get_by_name(childNode.name());
		if (type.is_valid() == false)
			continue;
		auto var = type.create();
		DeserializeNode(childNode, var);
		var.convert(view.get_value_type());
		const auto iter = view.insert(view.end(), var);
		if (iter == view.end())
		{
			CORE_LOG(E_Level::Error, E_Context::Core, "FAILED.");
		}
		index++;
	}
}

//=================================================================================
void C_XMLDeserializer::DeserializeAssociativeArray(const pugi::xml_node& child, rttr::variant_associative_view& view)
{
	auto keyType = view.get_key_type();
	for (const auto childNode : child) {
		rttr::variant keyVar;
		if (IsAtomicType(keyType)) {
			keyVar = DeserializeAtomic(childNode.child("key").attribute("value"), keyType);
			if (!keyVar.is_valid())
				CORE_LOG(E_Level::Error, E_Context::Core, "FAILED.");
		}
		view.insert(keyVar, rttr::variant(nullptr));
	}
}

//=================================================================================
rttr::variant C_XMLDeserializer::DeserializeAtomic(const pugi::xml_attribute& attr, const rttr::type& type)
{
	if (attr)
	{
		if (type == rttr::type::get<bool>())
			return attr.as_bool();
		else if (type == rttr::type::get<char>())
			return attr.as_bool();
		else if (type == rttr::type::get<int8_t>())
			return attr.as_int();
		else if (type == rttr::type::get<int16_t>())
			return attr.as_int();
		else if (type == rttr::type::get<int32_t>())
			return attr.as_int();
		else if (type == rttr::type::get<int64_t>())
			return attr.as_llong();
		else if (type == rttr::type::get<uint8_t>())
			return attr.as_uint();
		else if (type == rttr::type::get<uint16_t>())
			return attr.as_uint();
		else if (type == rttr::type::get<uint32_t>())
			return attr.as_uint();
		else if (type == rttr::type::get<uint64_t>())
			return attr.as_ullong();
		else if (type == rttr::type::get<float>())
			return attr.as_float();
		else if (type == rttr::type::get<double>())
			return attr.as_double();
		else if (type == rttr::type::get<std::string>())
			return std::string(attr.as_string());
		else if (type.is_enumeration())
			return type.get_enumeration().name_to_value(attr.as_string());
	}
	CORE_LOG(E_Level::Error, E_Context::Core, "Unknown atomics.");
	return {};
}

} // namespace GLEngine::Utils
