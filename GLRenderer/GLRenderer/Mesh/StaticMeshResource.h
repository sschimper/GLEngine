#pragma once

#include <Renderer/IResource.h>

#include <GLRenderer/VAO/VAO.h>

#include <glad/glad.h>

#include <array>

namespace GLEngine {
namespace Renderer::MeshData
{
struct Mesh;
}

namespace GLRenderer {
namespace Mesh {
class C_StaticMeshResource : public Renderer::I_Resource {
public:
	C_StaticMeshResource(const Renderer::MeshData::Mesh & mesh);
	~C_StaticMeshResource() = default;

	//=================================================================================
	virtual std::unique_ptr<Renderer::I_RawGPUData> ExtractData() const override;
	virtual void Invalidate() override;
	virtual bool IsValid() override;

	void BindVAO() const;
	std::size_t GetNumTriangles() const;
protected:
	VAO::C_GLVAO<5> m_VAO;
	std::size_t m_triangles;
};


}}}