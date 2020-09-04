#include <GLRendererStdafx.h>

#include <GLRenderer/Components/StaticMesh.h>

#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Buffers/UBO/ModelData.h>
#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/Textures/TextureUnitManager.h>
#include <GLRenderer/Textures/TextureManager.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>

#include <Renderer/Mesh/Loading/SceneLoader.h>
#include <Renderer/Materials/MaterialManager.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Scene.h>

#include <Core/Application.h>

#include <Utils/Parsing/MaterialParser.h>

#include <pugixml.hpp>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
C_StaticMesh::C_StaticMesh(std::string meshFile, std::string_view shader, std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_RenderableComponent(owner)
	, m_meshFile(meshFile)
	, m_Mesh(nullptr)
	, m_Material(nullptr)
{
	// @todo lazy init
	auto sl = std::make_unique<Renderer::Mesh::SceneLoader>();

	auto scene = std::make_shared<Renderer::MeshData::Scene>();
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	if (!sl->addModelFromFileToScene("Models", m_meshFile.c_str(), scene, modelMatrix))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unable to load model {}", m_meshFile);
		return;
	}
	m_ComponentMatrix = modelMatrix;

	// TODO this is unsafe way to init model
	m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(scene->meshes[0]);
	m_AABB = scene->meshes[0].bbox;
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader = shmgr.GetProgram(shader.data());

	const auto materialIdx = scene->meshes[0].materialIndex;
	const auto& material = scene->materials[materialIdx];

	SetMaterial(material);
}

//=================================================================================
C_StaticMesh::C_StaticMesh(const Renderer::MeshData::Mesh& mesh, std::string_view shader, std::shared_ptr<Entity::I_Entity> owner, const Renderer::MeshData::Material* material)
	: Renderer::I_RenderableComponent(owner)
	, m_Material(nullptr)
{
	m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(mesh);
	m_AABB = mesh.bbox;

	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader = shmgr.GetProgram(shader.data());

	if (material)
		SetMaterial(*material);
}

//=================================================================================
void C_StaticMesh::PerformDraw() const
{
	if (!m_Mesh || !m_Shader)
	{
		//CORE_LOG(E_Level::Error, E_Context::Render, "Static mesh uncomplete");
		return;
	}
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();


	auto& shmgr = Shaders::C_ShaderManager::Instance();
	auto& tmgr = Textures::C_TextureManager::Instance();
	if (renderer->GetCurrentPassType() == Renderer::E_PassType::ShadowPass && m_ShadowPassShader)
	{
		shmgr.ActivateShader(m_ShadowPassShader);
	}
	else
	{
		shmgr.ActivateShader(m_Shader);
	}

	auto& tm = Textures::C_TextureUnitManger::Instance();

	if (m_RoughnessMap) {
		tm.BindTextureToUnit(*m_RoughnessMap, 0);
	}
	else
	{
		tm.BindTextureToUnit(*(tmgr.GetIdentityTexture()), 0);
	}
	if (m_ColorMap) {
		tm.BindTextureToUnit(*m_ColorMap, 1);
	}
	else
	{
		tm.BindTextureToUnit(*(tmgr.GetIdentityTexture()), 1);
	}
	if (m_NormalMap) {
		tm.BindTextureToUnit(*m_NormalMap, 2);
	}
	else
	{
		tm.BindTextureToUnit(*(tmgr.GetIdentityTexture()), 2);
	}

	assert(m_Material->GetMaterialIndex() >= 0);
	renderer->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&, matIndex = m_Material?m_Material->GetMaterialIndex():0]() {
					auto modelData = Buffers::C_UniformBuffersManager::Instance().GetBufferByName("modelData");
					if (auto modelDataUbo = std::dynamic_pointer_cast<Buffers::UBO::C_ModelData>(modelData))
					{
						modelDataUbo->SetModelMatrix(GetComponentModelMatrix());
						modelDataUbo->SetMaterialIndex(matIndex);
						modelDataUbo->UploadData();
					}
				}
				, "Per object data upload")
		)
	);

	renderer->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_Mesh)
		)
	);
}

//=================================================================================
void C_StaticMesh::DebugDrawGUI()
{
	if (::ImGui::CollapsingHeader("Static mesh")) {
		m_Color.Draw();
		if (!m_RoughnessMap)
		{
			m_Roughness.Draw();
		}
	}
}

//=================================================================================
void C_StaticMesh::SetMaterial(const Renderer::MeshData::Material& material)
{
	auto& materialManager = Renderer::C_MaterialManager::Instance();
	m_Material = materialManager.GetMaterial(material.m_Name);
	if (!m_Material)
	{
		m_Material = materialManager.RegisterMaterial(Renderer::C_Material(material));
	}
}

//=================================================================================
const GLEngine::Physics::Primitives::S_AABB& C_StaticMesh::GetAABB() const
{
	return m_AABB;
}

//=================================================================================
std::shared_ptr<Entity::I_Component> C_StaticMeshBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	const auto materialData = Utils::Parsing::C_MaterialParser::ParseMaterialData(node);

	auto staticMesh = std::make_shared<C_StaticMesh>(node.attribute("filePath").value(), materialData.m_MaterialName, owner);

	if (auto shadowPassAttr = node.attribute("shadowPassShader"))
	{
		auto& shmgr = Shaders::C_ShaderManager::Instance();
		auto shadowPassShader = shmgr.GetProgram(shadowPassAttr.as_string());
		if (shadowPassShader)
		{
			staticMesh->m_ShadowPassShader = shadowPassShader;
		}
	}

	staticMesh->SetColor(materialData.m_Color);
	staticMesh->m_Roughness = materialData.m_Roughness;


	auto& tmgr = Textures::C_TextureManager::Instance();

	if (!materialData.m_RoughtnessMap.empty())
	{
		auto roughnessMap = tmgr.GetTexture(materialData.m_RoughtnessMap);
		if (roughnessMap)
		{
			roughnessMap->StartGroupOp();
			roughnessMap->SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
			roughnessMap->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			roughnessMap->GenerateMipMaps();

			roughnessMap->EndGroupOp();

			staticMesh->SetRoughnessMap(roughnessMap);
		}
	}

	if (!materialData.m_ColorMap.empty())
	{
		auto colorMapTexture = tmgr.GetTexture(materialData.m_ColorMap);
		if (colorMapTexture)
		{
			colorMapTexture->StartGroupOp();
			colorMapTexture->SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
			colorMapTexture->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			colorMapTexture->GenerateMipMaps();
		
			colorMapTexture->EndGroupOp();

			staticMesh->SetColorMap(colorMapTexture);
		}
	}

	if (!materialData.m_NormalMap.empty())
	{
		staticMesh->m_NormalMap = tmgr.GetTexture(materialData.m_NormalMap);
		if (staticMesh->m_NormalMap)
		{
			staticMesh->m_NormalMap->StartGroupOp();
			staticMesh->m_NormalMap->SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
			staticMesh->m_NormalMap->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			staticMesh->m_NormalMap->GenerateMipMaps();

			staticMesh->m_NormalMap->EndGroupOp();
		}
	}

	return staticMesh;
}

}