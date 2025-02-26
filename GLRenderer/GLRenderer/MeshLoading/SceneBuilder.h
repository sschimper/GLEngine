/**
 * ==============================================
 * \file 		SceneBuilder.h
 * \date 		2018/03/17 19:19
 * \project 	Computer Graphics Project
 * \faculty 	Faculty of Information Technology 
 * \university 	Brno University of Technology
 *
 * \author Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * ==============================================
 */
 
#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <vector>
#include <string>


namespace pugi {
class xml_document;
class xml_node;
}


namespace GLEngine {
namespace GLRenderer {

namespace Textures {
class C_Texture;
}

namespace Mesh {

struct Scene;
struct Mesh;
struct Texture;
class I_RenderNode;
class C_Scene;
class C_Terrain;
//@todo needs refactor
class C_SceneBuilder {
public:
	C_SceneBuilder();
	~C_SceneBuilder() = default;

	std::shared_ptr<C_Scene> LoadScene(const std::string& sceneDefinitionFile);
	std::shared_ptr<C_Terrain> LoadTerrain(const pugi::xml_node& node);
	std::shared_ptr<C_Scene> LoadModel(const pugi::xml_node& node);
	std::shared_ptr<I_RenderNode> LoadMesh(const Mesh& mesh);
	std::shared_ptr<Textures::C_Texture> LoadTexture(const Texture& texture) const;
private:
	glm::vec3	ReadPositionNode(const pugi::xml_node& node) const noexcept;
	std::string GetFolderpath(const std::string& filePath) const;
	std::string GetFilePart(const std::string& filePath) const;

	std::vector<std::shared_ptr<Textures::C_Texture>> m_textures;
	std::shared_ptr<Textures::C_Texture> m_nullTexture;
	std::shared_ptr<Scene> m_scene;
	std::string m_sceneFolder;
};

}}}