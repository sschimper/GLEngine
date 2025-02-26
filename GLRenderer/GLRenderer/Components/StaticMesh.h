#pragma once

#include <Renderer/IRenderableComponent.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>

namespace GLEngine {
namespace GLRenderer {

namespace Mesh {
struct Mesh;
}

namespace Components {

class C_StaticMesh : public Renderer::I_RenderableComponent {
public:
	C_StaticMesh(std::string meshFile);
	C_StaticMesh(const Mesh::Mesh& meshFile);
	virtual void PerformDraw() const override;
protected:
	std::string										m_meshFile;
	std::shared_ptr<Mesh::C_StaticMeshResource>		m_Mesh;
};

class C_StaticMeshBuilder : public Entity::I_ComponenetBuilder
{
public:
	virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node) override;
};

}}}