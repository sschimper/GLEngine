#include <Physics/Primitives/Frustum.h>

#include <Physics/Primitives/AABB.h>


namespace GLEngine {
namespace Physics {
namespace Primitives {

//=================================================================================
C_Frustum::C_Frustum(const glm::vec3& position, const glm::vec3& upVectro, const glm::vec3& foreward, float near, float far, float aspect, float fov)
	: m_position(position)
	, m_upVector(upVectro)
	, m_foreward(foreward)
	, m_near(near)
	, m_far(far)
	, m_aspect(aspect)
	, m_fov(fov)
{}

//=================================================================================
S_AABB C_Frustum::GetAABB() const
{
	glm::vec3 nearCenter = glm::vec3(m_position + (m_foreward * m_near));
	glm::vec3 farCenter = glm::vec3(m_position + (m_foreward * m_far));

	glm::vec3 left = glm::cross(m_upVector, m_foreward);

	float tanHalfHFOV = tanf(glm::radians(m_fov / 2.0f));
	float tanHalfVFOV = tanf(glm::radians((m_fov * m_aspect) / 2.0f));

	float xn = m_near * tanHalfVFOV;
	float xf = m_far * tanHalfVFOV;
	float yn = m_near * tanHalfHFOV;
	float yf = m_far * tanHalfHFOV;

	glm::vec4 nlt = glm::vec4(nearCenter + (xn * left) + m_upVector * yn, 1.0f);
	glm::vec4 nrt = glm::vec4(nearCenter - (xn * left) + m_upVector * yn, 1.0f);
	glm::vec4 nlb = glm::vec4(nearCenter + (xn * left) - m_upVector * yn, 1.0f);
	glm::vec4 nrb = glm::vec4(nearCenter - (xn * left) - m_upVector * yn, 1.0f);
	glm::vec4 flt = glm::vec4(farCenter + (xf * left) + m_upVector * yf, 1.0f);
	glm::vec4 frt = glm::vec4(farCenter - (xf * left) + m_upVector * yf, 1.0f);
	glm::vec4 flb = glm::vec4(farCenter + (xf * left) - m_upVector * yf, 1.0f);
	glm::vec4 frb = glm::vec4(farCenter - (xf * left) - m_upVector * yf, 1.0f);

	S_AABB bbox;
	bbox.Add(nlt);
	bbox.Add(nrt);
	bbox.Add(nlb);
	bbox.Add(nrb);
	bbox.Add(flt);
	bbox.Add(frt);
	bbox.Add(flb);
	bbox.Add(frb);
	return bbox;
}

//=================================================================================
void C_Frustum::UpdateWithMatrix(const glm::mat4& matrix)
{
	auto newPosition = glm::vec3((matrix * glm::vec4(m_position, 1.0f)));
	m_upVector = glm::vec3((matrix * glm::vec4(m_position + m_upVector, 1.0f))) - newPosition;
	m_foreward = glm::vec3((matrix * glm::vec4(m_position + m_foreward, 1.0f))) - newPosition;

	m_position = newPosition;
}
}
}
}