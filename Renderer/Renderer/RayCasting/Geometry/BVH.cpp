#include <RendererStdafx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/RayCasting/Geometry/BVH.h>

#include <Physics/GeometryUtils/TriangleIntersect.h>

namespace GLEngine::Renderer {

//=================================================================================
BVH::BVH(std::vector<glm::vec3>& storage)
	: m_Storage(storage)
{
	Build();
}

//=================================================================================
BVH::~BVH() = default;

//=================================================================================
void BVH::Build()
{
	if (m_Storage.empty())
		return;

	GLE_ASSERT(m_Nodes.empty(), "Trying to rebuild the BVH without cleaning.");

	auto& root = m_Nodes.emplace_back();
	for (const auto& vertex : m_Storage)
		root.aabb.Add(vertex);
	root.firstTrig = 0;
	root.lastTrig  = static_cast<unsigned int>(m_Storage.size()) - 3;
	SplitBVHNodeNaive(0, 0);
}

//=================================================================================
void BVH::SplitBVHNodeNaive(T_BVHNodeID nodeId, unsigned int level)
{
	if (level > 10)
		return;

	// limit nodes thats too small
	if (m_Nodes[nodeId].lastTrig - m_Nodes[nodeId].firstTrig <= 20 * 3)
		return;

	auto axis = level % 3;

	// naive split!
	// 1) Allocate two new nodes
	m_Nodes.emplace_back();
	const T_BVHNodeID leftNodeId = static_cast<T_BVHNodeID>(m_Nodes.size()) - 1;
	m_Nodes[nodeId].left		 = leftNodeId;
	m_Nodes.emplace_back();
	const T_BVHNodeID rightNodeId = static_cast<T_BVHNodeID>(m_Nodes.size()) - 1;
	m_Nodes[nodeId].right		  = rightNodeId;
	// now I can store references, because I am not adding new elements
	auto& left	= m_Nodes[leftNodeId];
	auto& right = m_Nodes[rightNodeId];
	auto& node	= m_Nodes[nodeId];

	// first split along X
	auto	   average	  = node.aabb.m_Min[axis] + (node.aabb.m_Max[axis] - node.aabb.m_Min[axis]) * 0.5f;
	const auto trigCenter = [](const glm::vec3* triDef, unsigned int axis) { return (triDef[0][axis] + triDef[1][axis] + triDef[2][axis]) / 3.f; };


	// try finding better than average
	float		 bestCost	 = 1e30f;
	float		 bestAverage = 0.f;
	unsigned int bestAxis	 = 0;
	for (unsigned int axe = 0; axe < 3; ++axe)
	{
		for (unsigned int i = node.firstTrig; i < node.lastTrig + 3; i += 3)
		{
			const glm::vec3* triDef			 = &(m_Storage[i]);
			const float		 currentCentroid = trigCenter(triDef, axe);
			const float		 cost			 = CalcSAHCost(node, axe, currentCentroid);
			if (cost < bestCost)
			{
				bestCost	= cost;
				bestAverage = currentCentroid;
				bestAxis	= axe;
			}
		}
	}
	axis	= bestAxis;
	average = bestAverage;

	unsigned int leftSorting = node.firstTrig, rightSorting = node.lastTrig;
	while (leftSorting < rightSorting)
	{
		// find left candidate to swap
		const glm::vec3* triDef = &(m_Storage[leftSorting]);
		if (trigCenter(triDef, axis) < average)
		{
			leftSorting += 3;
			continue;
		}
		// find right candidate for swap
		while (true)
		{
			const glm::vec3* triDef = &(m_Storage[rightSorting]);
			if (trigCenter(triDef, axis) < average)
			{
				break;
			}
			rightSorting -= 3;
		}
		if (leftSorting >= rightSorting) {
			std::swap(leftSorting, rightSorting);
			break;
		}
		// swap
		std::swap(m_Storage[leftSorting], m_Storage[rightSorting]);
		std::swap(m_Storage[leftSorting + 1], m_Storage[rightSorting + 1]);
		std::swap(m_Storage[leftSorting + 2], m_Storage[rightSorting + 2]);

		// leftSorting += 3;
		// rightSorting -= 3;
	}

	left.firstTrig	= node.firstTrig;
	left.lastTrig	= leftSorting;
	right.firstTrig = leftSorting + 3; // not using rightSorting to count in all triangles
	right.lastTrig	= node.lastTrig;

	GLE_ASSERT(left.NumTrig() + right.NumTrig() == node.NumTrig(), "The children nodes triangels sum should be equal to parent trigs.");

	// build AABBs
	for (unsigned int i = left.firstTrig; i < left.lastTrig + 3; ++i)
	{
		left.aabb.Add(m_Storage[i]);
	}
	for (unsigned int i = right.firstTrig; i < right.lastTrig + 3; ++i)
	{
		right.aabb.Add(m_Storage[i]);
	}

	SplitBVHNodeNaive(leftNodeId, level + 1);
	SplitBVHNodeNaive(rightNodeId, level + 1);
}

//=================================================================================
bool BVH::Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
{
	return IntersectNode(ray, intersection, &m_Nodes[0]);
}

//=================================================================================
bool BVH::IntersectNode(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, const BVHNode* node) const
{
	if (node->aabb.IntersectImpl(ray) <= 0.f)
		return false;
	else
	{
		if (node->left != s_InvalidBVHNode || node->right != s_InvalidBVHNode)
		{
			// we can have hit from both sides as sides can overlap
			C_RayIntersection intersections[2];
			bool			  intersectionsResults[2] = {false, false};
			if (node->left != s_InvalidBVHNode)
				intersectionsResults[0] = IntersectNode(ray, intersections[0], &m_Nodes[node->left]);
			if (node->right != s_InvalidBVHNode)
				intersectionsResults[1] = IntersectNode(ray, intersections[1], &m_Nodes[node->right]);
			// find closer intersection
			if (intersectionsResults[0] && intersectionsResults[1])
			{
				if (intersections[0].GetRayLength() < intersections[1].GetRayLength())
				{
					intersection = intersections[0];
				}
				else
				{
					intersection = intersections[1];
				}
			}
			else if (intersectionsResults[0])
			{
				intersection = intersections[0];
			}
			else if (intersectionsResults[1])
			{
				intersection = intersections[1];
			}
			return intersectionsResults[0] || intersectionsResults[1];
		}
	}
	// we are in the leaf node
	struct S_IntersectionInfo {
		C_RayIntersection intersection;
		float			  t;

		[[nodiscard]] bool operator<(const S_IntersectionInfo& a) const { return t < a.t; }
	};
	S_IntersectionInfo closestIntersect{C_RayIntersection(), std::numeric_limits<float>::max()};

	glm::vec2 barycentric;

	for (unsigned int i = node->firstTrig; i <= node->lastTrig; i += 3)
	{
		const glm::vec3* triDef = &(m_Storage[i]);
		const auto		 length = Physics::TraingleRayIntersect(triDef, ray, &barycentric);
		if (length > 0.0f)
		{
			if (closestIntersect.t < length)
			{
				continue;
			}
			auto	   normal = glm::cross(m_Storage[i + 1] - m_Storage[i], m_Storage[i + 2] - m_Storage[i]);
			const auto area	  = glm::length(normal) / 2.f;
			normal			  = glm::normalize(normal);
			C_RayIntersection inter(S_Frame(normal), ray.origin + length * ray.direction, Physics::Primitives::S_Ray(ray));
			inter.SetRayLength(length);

			closestIntersect = {inter, length};
		}
	}
	if (closestIntersect.t == std::numeric_limits<float>::max())
		return false;


	intersection = closestIntersect.intersection;
	return true;
}

//=================================================================================
void BVH::DebugDraw(I_DebugDraw* dd, const glm::mat4& modelMatrix) const
{
	if (m_Nodes.empty())
		return;
	DebugDrawNode(dd, modelMatrix, &m_Nodes[0], 0);
}

//=================================================================================
void BVH::DebugDrawNode(I_DebugDraw* dd, const glm::mat4& modelMatrix, const BVHNode* node, unsigned int level) const
{
	// can be simple loop
	if (!node)
		return;
	static Colours::T_Colour colours[] = {
		Colours::black,
		Colours::green,
		Colours::blue,
		Colours::red,
		Colours::gray,
		Colours::purple,
		Colours::yellow,
		Colours::white,
	};
	constexpr static auto numColours			   = (sizeof(colours) / sizeof(Colours::T_Colour));
	const Colours::T_Colour& currentColour = colours[level < numColours ? level : numColours - 1];
	dd->DrawAABB(node->aabb, currentColour, modelMatrix);
	if (node->left != s_InvalidBVHNode)
		DebugDrawNode(dd, modelMatrix, &m_Nodes[node->left], level + 1);
	if (node->right != s_InvalidBVHNode)
		DebugDrawNode(dd, modelMatrix, &m_Nodes[node->right], level + 1);
}

//=================================================================================
float BVH::CalcSAHCost(const BVHNode& parent, const unsigned int axis, const float splitPos) const
{
	const auto trigCenter = [&](const glm::vec3* triDef) { return (triDef[0][axis] + triDef[1][axis] + triDef[2][axis]) / 3.f; };

	Physics::Primitives::S_AABB left, right;
	unsigned int				leftCount = 0, rightCount = 0;
	for (unsigned int i = parent.firstTrig; i < parent.lastTrig + 3; i += 3)
	{
		const glm::vec3* triDef = &(m_Storage[i]);
		if (trigCenter(triDef) < splitPos)
		{
			left.Add(triDef[0]);
			left.Add(triDef[1]);
			left.Add(triDef[2]);
			++leftCount;
		}
		else
		{
			right.Add(triDef[0]);
			right.Add(triDef[1]);
			right.Add(triDef[2]);
			++rightCount;
		}
	}
	const float cost = leftCount * left.Area() + rightCount * right.Area();
	return cost > 0.f ? cost : 1e30f;
}

} // namespace GLEngine::Renderer
