#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/ModelData.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Components/StaticMesh.h>
#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureManager.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/Mesh/Loading/SceneLoader.h>
#include <Renderer/Mesh/Scene.h>

#include <Core/Application.h>

#include <Utils/Parsing/MaterialParser.h>

#include <pugixml.hpp>

#include <imgui.h>
#include <rttr/registration>

RTTR_REGISTRATION
{
	using namespace GLEngine::GLRenderer::Components;
	rttr::registration::class_<C_StaticMeshBuilder>("C_StaticMeshBuilder")
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.method("Build", &C_StaticMeshBuilder::Build);


	rttr::registration::class_<C_StaticMesh>("C_StaticMesh")
		.constructor<std::string, std::string_view, std::shared_ptr<GLEngine::Entity::I_Entity>>()
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("MeshFile", &C_StaticMesh::m_meshFile)
		.property("Material", &C_StaticMesh::m_Material)
		.property("Shader", &C_StaticMesh::GetShader, &C_StaticMesh::SetShader)
		.property("ShadowPassShader", &C_StaticMesh::GetShadowShader, &C_StaticMesh::SetShadowShader);
	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_StaticMesh>>();
	rttr::type::register_converter_func([](std::shared_ptr<C_StaticMesh> ptr, bool& ok) -> std::shared_ptr<GLEngine::Entity::I_Component> {
		ok = true;
		return std::static_pointer_cast<GLEngine::Entity::I_Component>(ptr);
	});
}

namespace GLEngine::GLRenderer::Components {

//=================================================================================
C_StaticMesh::C_StaticMesh(std::string meshFile, std::string_view shader, std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_RenderableComponent(owner)
	, m_meshFile(meshFile)
	, m_Mesh(nullptr)
	, m_Material(nullptr)
{
	SetShader(shader.data());
	Renderer::MeshData::Material material
	{
		glm::vec4{1.0f, 0.0f, 0.f, 0.f}, 
		glm::vec4{1.0f, 0.0f, 0.f, 0.f}, 
		glm::vec4{1.0f, 0.0f, 0.f, 0.f},
		.5f
	};
	SetMaterial(material);
}

//=================================================================================
C_StaticMesh::C_StaticMesh(const Renderer::MeshData::Mesh& mesh, std::string_view shader, std::shared_ptr<Entity::I_Entity> owner, const Renderer::MeshData::Material* material)
	: Renderer::I_RenderableComponent(owner)
	, m_Material(nullptr)
{
	m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(mesh);
	m_AABB = mesh.bbox;

	SetShader(shader.data());

	if (material)
		SetMaterial(*material);
}

//=================================================================================
C_StaticMesh::C_StaticMesh()
	: Renderer::I_RenderableComponent(nullptr)
	, m_Material(nullptr)
{
	SetShader("basicTracing"); // TODO
	Renderer::MeshData::Material material
	{
		glm::vec4{1.0f, 0.0f, 0.f, 0.f}, 
		glm::vec4{1.0f, 0.0f, 0.f, 0.f}, 
		glm::vec4{1.0f, 0.0f, 0.f, 0.f},
		.5f,
		-1, -1, 
		"Default"
	};
	SetMaterial(material);
}

//=================================================================================
void C_StaticMesh::PerformDraw() const
{
	if (!m_Mesh || !m_Shader)
	{
		const_cast<C_StaticMesh*>(this)->LoadMesh(); // lazy load, lazy dominik wrong design, should burn in hell
	}
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();


	auto& shmgr = Shaders::C_ShaderManager::Instance();
	auto& tmgr	= Textures::C_TextureManager::Instance();
	if (renderer.GetCurrentPassType() == Renderer::E_PassType::ShadowPass && m_ShadowPassShader)
	{
		shmgr.ActivateShader(m_ShadowPassShader);
	}
	else
	{
		shmgr.ActivateShader(m_Shader);
	}

	renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&, matIndex = m_Material ? m_Material->GetMaterialIndex() : 0]() {
			auto modelData = Buffers::C_UniformBuffersManager::Instance().GetBufferByName("modelData");
			if (auto modelDataUbo = std::dynamic_pointer_cast<Buffers::UBO::C_ModelData>(modelData))
			{
				modelDataUbo->SetModelMatrix(GetComponentModelMatrix());
				modelDataUbo->SetMaterialIndex(matIndex);
				modelDataUbo->UploadData();
			}
		},
		"Per object data upload"));

	renderer.AddCommand(std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_Mesh));
}

//=================================================================================
void C_StaticMesh::DebugDrawGUI()
{
	if (::ImGui::CollapsingHeader("Material"))
	{
		m_Material->DrawGUI();
	}
}

//=================================================================================
void C_StaticMesh::SetMaterial(const Renderer::MeshData::Material& material)
{
	auto& materialManager = Renderer::C_MaterialManager::Instance();
	m_Material			  = materialManager.GetMaterial(material.m_Name);
	if (!m_Material)
	{
		m_Material = materialManager.RegisterMaterial(Renderer::C_Material(material));
	}
}

//=================================================================================
void C_StaticMesh::SetMaterial(std::shared_ptr<Renderer::C_Material> material)
{
	m_Material = material;
}

//=================================================================================
std::string_view C_StaticMesh::GetDebugComponentName() const
{
	return "Static mesh";
}

//=================================================================================
bool C_StaticMesh::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
GLEngine::Physics::Primitives::S_AABB C_StaticMesh::GetAABB() const
{
	return m_AABB;
}

//=================================================================================
void C_StaticMesh::SetShader(const std::string shader)
{
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader	= shmgr.GetProgram(shader);
}

//=================================================================================
std::string C_StaticMesh::GetShader() const
{
	if (!m_Shader)
		return "";
	else
		return m_Shader->GetName();
}

//=================================================================================
void C_StaticMesh::SetShadowShader(const std::string shader)
{
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_ShadowPassShader	= shmgr.GetProgram(shader);
}

//=================================================================================
std::string C_StaticMesh::GetShadowShader() const
{
	if (!m_ShadowPassShader)
		return "";
	else
		return m_ShadowPassShader->GetName();
}

//=================================================================================
void C_StaticMesh::LoadMesh()
{
	Renderer::Mesh::SceneLoader sl;

	auto	  scene		  = std::make_shared<Renderer::MeshData::Scene>();
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	if (!sl.addModelFromFileToScene("Models", m_meshFile.c_str(), scene, modelMatrix))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unable to load model {}", m_meshFile);
		return;
	}

	m_Transformation.SetMatrix(modelMatrix);

	// TODO this is unsafe way to init model
	m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(scene->meshes[0]);
	m_AABB = scene->meshes[0].bbox;

	const auto materialIdx = scene->meshes[0].materialIndex;
	auto&	   material	   = scene->materials[materialIdx];

	SetMaterial(material);
}

//=================================================================================
std::shared_ptr<Entity::I_Component> C_StaticMeshBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	const auto materialData = Utils::Parsing::C_MaterialParser::ParseMaterialData(node);

	auto staticMesh = std::make_shared<C_StaticMesh>(node.attribute("filePath").value(), materialData.m_MaterialName, owner);

	if (auto shadowPassAttr = node.attribute("shadowPassShader"))
	{
		auto& shmgr			   = Shaders::C_ShaderManager::Instance();
		auto  shadowPassShader = shmgr.GetProgram(shadowPassAttr.as_string());
		if (shadowPassShader)
		{
			staticMesh->m_ShadowPassShader = shadowPassShader;
		}
	}
	auto& materialManager = Renderer::C_MaterialManager::Instance();
	auto  material		  = materialManager.GetMaterial(owner->GetName() + "_Material");
	if (!material)
	{
		material = materialManager.RegisterMaterial(Renderer::C_Material(owner->GetName() + "_Material"));
	}

	material->SetColor(materialData.m_Color);
	material->SetRoughness(materialData.m_Roughness);

	auto& tmgr = Textures::C_TextureManager::Instance();

	if (!materialData.m_RoughtnessMap.empty())
	{
		auto roughnessMap = tmgr.GetTexture(materialData.m_RoughtnessMap);
		if (roughnessMap)
		{
			roughnessMap->SetWrap(Renderer::E_WrapFunction::Repeat, Renderer::E_WrapFunction::Repeat);
			roughnessMap->SetFilter(Renderer::E_TextureFilter::LinearMipMapLinear, Renderer::E_TextureFilter::Linear);
			roughnessMap->SetParameter(GL_TEXTURE_COMPARE_MODE, GL_NONE);
			roughnessMap->GenerateMipMaps();

			material->SetRoughnessMap(roughnessMap);
		}
	}

	if (!materialData.m_ColorMap.empty())
	{
		auto colorMapTexture = tmgr.GetTexture(materialData.m_ColorMap);
		if (colorMapTexture)
		{
			colorMapTexture->SetWrap(Renderer::E_WrapFunction::Repeat, Renderer::E_WrapFunction::Repeat);
			colorMapTexture->SetFilter(Renderer::E_TextureFilter::LinearMipMapLinear, Renderer::E_TextureFilter::Linear);
			colorMapTexture->SetParameter(GL_TEXTURE_COMPARE_MODE, GL_NONE);
			colorMapTexture->GenerateMipMaps();

			material->SetColorMap(colorMapTexture);
		}
	}

	if (!materialData.m_NormalMap.empty())
	{
		auto normalMap = tmgr.GetTexture(materialData.m_NormalMap);
		if (normalMap)
		{
			normalMap->SetWrap(Renderer::E_WrapFunction::Repeat, Renderer::E_WrapFunction::Repeat);
			normalMap->SetFilter(Renderer::E_TextureFilter::LinearMipMapLinear, Renderer::E_TextureFilter::Linear);
			normalMap->SetParameter(GL_TEXTURE_COMPARE_MODE, GL_NONE);
			normalMap->GenerateMipMaps();

			material->SetNormalMap(normalMap);
		}
	}

	staticMesh->SetMaterial(material);

	return staticMesh;
}

} // namespace GLEngine::GLRenderer::Components