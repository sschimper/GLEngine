#pragma once

#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/Primitives/AABB.h>

namespace GLEngine::Renderer {
class I_DebugDraw;

class BVH {
public:
	// BVH is not owning struct, but can modify order of elements in the storage
	BVH(std::vector<glm::vec3>& storage);
	~BVH();
	[[nodiscard]] bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const;

	void DebugDraw(I_DebugDraw* dd, const glm::mat4& modelMatrix) const;

private:
	void Build();

	using T_BVHNodeID = unsigned short;
	struct BVHNode {
		Physics::Primitives::S_AABB aabb;
		T_BVHNodeID					left  = -1;
		T_BVHNodeID					right = -1;
		unsigned int				firstTrig, lastTrig; // index of first vertex
	};
	[[nodiscard]] bool IntersectNode(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, const BVHNode* node) const;
	void			   DebugDrawNode(I_DebugDraw* dd, const glm::mat4& modelMatrix, const BVHNode* node) const;
	// using NodeID because the vector is being reallocated on the way
	void SplitBVHNode(T_BVHNodeID node, unsigned int level);

	std::vector<glm::vec3>& m_Storage;
	std::vector<BVHNode>	m_Nodes;
};
} // namespace GLEngine::Renderer
