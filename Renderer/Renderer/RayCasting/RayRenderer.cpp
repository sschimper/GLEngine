#include <RendererStdafx.h>

#include <Renderer/RayCasting/RayRenderer.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/Textures/TextureStorage.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {

//=================================================================================
C_RayRenderer::C_RayRenderer(const C_RayTraceScene& scene)
	: m_Scene(scene)
{

}

//=================================================================================
void C_RayRenderer::Render(I_CameraComponent& camera, I_TextureViewStorage& storage) const
{
	const auto low1 = 0.0;
	const auto high1 = 5.0f;
	const auto low2 = 0.0;
	const auto high2 = 255.0;
	const auto dim = storage.GetDimensions();

	const auto ToClipSpace = [&](const glm::vec2& screenCoord) -> glm::vec2
	{
		const float x = (2.0f * screenCoord.x) / dim.x - 1.0f;
		const float y = 1.0f - (2.0f * screenCoord.y) / dim.y;
		return { x, y };
	};
	const auto Map = [&](float val)
	{
		return low2 + (glm::clamp(val, 0.0f, high1) - low1) * (high2 - low2) / (high1 - low1);
	};

	const auto GetRay = [&](int x, int y) {
		const auto clipSpace = ToClipSpace({ static_cast<float>(x), static_cast<float>(y) });
		return camera.GetRay(clipSpace);
	};

	auto textureView = Renderer::C_TextureView(&storage);

	const auto eps = 1e-5f;
	auto min = std::numeric_limits<float>::max();
	auto max = std::numeric_limits<float>::min();

	const auto pointLightPosition = glm::vec3(-1, 1, -1);
	const auto pointLightIntensity = 5.f;

	const glm::vec3 lightNormal = glm::normalize(glm::vec3(0, -0.5f, -1));
	const C_Primitive<Physics::Primitives::S_Disc> areaLight(Physics::Primitives::S_Disc(lightNormal, glm::vec3(0, 1, 2), .25f));
	const auto areLightPower = 255.f;

	for (int y = 0; y < dim.y; ++y)
	{
		for (int x = 0; x < dim.x; ++x)
		{
			const auto ray = GetRay(x, y);
			C_RayIntersection intersect;

			if (areaLight.Intersect(ray, intersect))
			{
				textureView.Set({ x,y }, glm::vec3{ areLightPower ,areLightPower ,areLightPower });
				continue;
			}

			if (!m_Scene.Intersect(ray, intersect)) continue;

			const auto frame = intersect.GetFrame();
			const auto toLight = pointLightPosition - intersect.GetIntersectionPoint();
			const auto cosTheta = frame.CosTheta(frame.ToLocal(glm::normalize(toLight)));
			const auto sample = intersect.GetMaterial()->diffuse * cosTheta * (pointLightIntensity / glm::dot(toLight, toLight));

			// Light sample (Single bounce - only hard shadows)
			C_RayIntersection intersectLight;
			const Physics::Primitives::S_Ray lightRay{ pointLightPosition, glm::normalize(-toLight) };
			if (!m_Scene.Intersect(lightRay, intersectLight))
				CORE_LOG(E_Level::Debug, E_Context::Render, "Error {} {}", x, y); // please, please don't fail

			if(intersectLight.GetRayLength() < glm::distance(intersect.GetIntersectionPoint(), pointLightPosition) - eps)
				continue;

			textureView.Set({ x,y }, textureView.Get<glm::vec3>({ x,y }) + glm::vec3{ sample.x, sample.y, sample.z }); // Map due to texture format

			// need for mapping purposes
			min = std::min(min, sample.x);
			min = std::min(min, sample.y);

			max = std::max(max, sample.x);
			max = std::max(max, sample.y);
		}
	}

	CORE_LOG(E_Level::Debug, E_Context::Render, "{} {}", min, max);
}

}