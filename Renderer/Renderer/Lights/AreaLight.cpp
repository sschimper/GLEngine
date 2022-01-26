#include <RendererStdafx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/Lights/AreaLight.h>

#include <GUI/ReflectionGUI.h>

#include <Physics/Primitives/Frustum.h>

#include <Utils/Parsing/ColorParsing.h>
#include <Utils/Parsing/MatrixParse.h>

#include <rttr/registration>

#pragma region registration
RTTR_REGISTRATION
{
	using namespace Utils::Reflection;
	using namespace GLEngine::Renderer;
	
	rttr::registration::class_<C_AreaLight>("C_AreaLight")
		.constructor<std::shared_ptr<GLEngine::Entity::I_Entity>>()
		.property("Width", &C_AreaLight::m_Width)
			(
				rttr::policy::prop::bind_as_ptr,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Width:"),
				RegisterMetamember<UI::Slider::Min>(.1f),
				RegisterMetamember<UI::Slider::Max>(10.0f),
				RegisterMetamember<SerializationCls::DerefSerialize>(true)
			)
		.property("Height", &C_AreaLight::m_Height)
			(
				rttr::policy::prop::bind_as_ptr,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Height:"),
				RegisterMetamember<UI::Slider::Min>(.1f),
				RegisterMetamember<UI::Slider::Max>(10.0f),
				RegisterMetamember<SerializationCls::DerefSerialize>(true)
			)
		.property("DiffuseColour", &C_AreaLight::m_DiffuseColor)
			(
				rttr::policy::prop::bind_as_ptr,
				RegisterMetaclass<MetaGUI::Colour>(),
				RegisterMetamember<UI::Colour::Name>("Diffuse colour:")
			)
		.property("SpecColour", &C_AreaLight::m_SpecularColor)
			(
				rttr::policy::prop::bind_as_ptr,
				RegisterMetaclass<MetaGUI::Colour>(),
				RegisterMetamember<UI::Colour::Name>("Spec colour:")
			);
}
#pragma endregion registration


namespace GLEngine::Renderer {

//=================================================================================
C_AreaLight::C_AreaLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_Light(owner)
	, m_Width(1.f)
	, m_Height(1.f)
	, m_DiffuseColor(Colours::white)
	, m_SpecularColor(Colours::white)
{
}

//=================================================================================
C_AreaLight::~C_AreaLight() = default;

//=================================================================================
Physics::Primitives::C_Frustum C_AreaLight::GetShadingFrustum() const
{
	auto						   transformMatrix = GetComponentModelMatrix();
	Physics::Primitives::C_Frustum ret(transformMatrix[3], GetUpVector(), GetNormal(), 0.1f, 50.f, 1.0f, 0.f);
	return ret;
}

//=================================================================================
Physics::Primitives::S_AABB C_AreaLight::GetAABB() const
{
	Physics::Primitives::S_AABB aabb;

	const auto dirY = GetUpVector();
	const auto dirX = glm::cross(GetNormal(), dirY);

	const auto width  = std::sqrt(GetWidth() / 2.0f);
	const auto height = std::sqrt(GetHeight() / 2.0f);

	aabb.Add(+dirY * height + dirX * width);
	aabb.Add(+dirY * height - dirX * width);
	aabb.Add(-dirY * height + dirX * width);
	aabb.Add(-dirY * height - dirX * width);

	return aabb;
}

//=================================================================================
void C_AreaLight::DebugDrawGUI()
{
	rttr::instance obj(*this);
	GUI::DrawAllPropertyGUI(obj);
}

//=================================================================================
std::string_view C_AreaLight::GetDebugComponentName() const
{
	return "Area light";
}

//=================================================================================
bool C_AreaLight::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
Colours::T_Colour C_AreaLight::DiffuseColour() const
{
	return m_DiffuseColor;
}

//=================================================================================
Colours::T_Colour C_AreaLight::SpecularColour() const
{
	return m_SpecularColor;
}

//=================================================================================
glm::vec3 C_AreaLight::GetNormal() const
{
	return GetComponentModelMatrix() * glm::vec4(0, 0, -1, 1);
}

//=================================================================================
glm::vec3 C_AreaLight::GetUpVector() const
{
	return GetComponentModelMatrix() * glm::vec4(0, 1, 0, 1);
}

//=================================================================================
void C_AreaLight::DebugDraw(Renderer::I_DebugDraw* dd) const
{
	const auto upVector = GetUpVector();
	const auto normal	= GetNormal();
	const auto dirX		= glm::cross(normal, upVector);
	const auto width	= std::sqrt(GetWidth() / 2.0f);
	const auto height	= std::sqrt(GetHeight() / 2.0f);

	auto transformMatrix = GetComponentModelMatrix();

	const auto Pos = glm::vec3(transformMatrix[3]);

	dd->DrawLine(Pos, Pos + normal, Colours::yellow);

	dd->DrawLine(Pos + upVector * height + dirX * width, Pos + upVector * height - dirX * width, Colours::yellow);
	dd->DrawLine(Pos - upVector * height + dirX * width, Pos - upVector * height - dirX * width, Colours::yellow);

	dd->DrawPoint(Pos + upVector * height + dirX * width, Colours::green);

	dd->DrawLine(Pos - upVector * height + dirX * width, Pos + upVector * height + dirX * width, Colours::yellow);
	dd->DrawLine(Pos - upVector * height - dirX * width, Pos + upVector * height - dirX * width, Colours::yellow);
}

//=================================================================================
[[nodiscard]] std::shared_ptr<Entity::I_Component> C_AreaLightCompBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	auto areaLight = std::make_shared<Renderer::C_AreaLight>(owner);
	areaLight->SetComponentMatrix(Utils::Parsing::C_MatrixParser::ParseTransformation(node));


	if (const auto widthAttr = node.attribute("width"))
	{
		areaLight->m_Width = widthAttr.as_float();
	}

	if (const auto heightAttr = node.attribute("height"))
	{
		areaLight->m_Height = heightAttr.as_float();
	}

	if (node.child("DiffuseColor"))
	{
		areaLight->m_DiffuseColor = Utils::Parsing::C_ColorParser::ParseColorRGB(node, "DiffuseColor");
	}

	if (node.child("SpecularColor"))
	{
		areaLight->m_SpecularColor = Utils::Parsing::C_ColorParser::ParseColorRGB(node, "SpecularColor");
	}

	return areaLight;
}

} // namespace GLEngine::Renderer