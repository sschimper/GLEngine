#pragma once

#include <Renderer/RendererApi.h>

namespace GLEngine::Physics::Primitives {
struct S_Ray;
}

namespace GLEngine::Renderer {
class C_RayIntersection;
class I_RayGeometryObject;

namespace RayTracing {
class I_RayLight;
class C_AreaLight;
} // namespace RayTracing

class RENDERER_API_EXPORT C_RayTraceScene {
public:
	C_RayTraceScene();
	C_RayTraceScene(const C_RayTraceScene&) = delete;
	~C_RayTraceScene();

	void operator=(const C_RayTraceScene&) = delete;

	[[nodiscard]] bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, float offset = 0.f) const;
	void			   AddObejct(std::shared_ptr<I_RayGeometryObject>&& object);
	void			   AddLight(std::shared_ptr<RayTracing::C_AreaLight>&& light);

	void ForEachLight(std::function<void(const RayTracing::C_AreaLight& light)> fnc) const;

private:
	std::vector<std::shared_ptr<I_RayGeometryObject>>	  m_Objects;
	std::vector<std::shared_ptr<RayTracing::C_AreaLight>> m_AreaLights;
};

} // namespace GLEngine::Renderer