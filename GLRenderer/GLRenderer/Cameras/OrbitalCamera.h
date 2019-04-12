#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Renderer/ICameraComponent.h>


namespace GLEngine {

namespace Core {
class C_KeyPressedEvent;
}

namespace Physics {
namespace Primitives {
class C_Frustum;
}
}

namespace GLRenderer {
namespace Cameras {

//Orbital camera
//Using Euclidean angles (= has gimbal lock)
class C_OrbitalCamera : public Renderer::I_CameraComponent
{
public:
	C_OrbitalCamera();
	~C_OrbitalCamera() = default;

	virtual glm::mat4 GetViewProjectionMatrix() const override;
	virtual glm::mat4 GetProjectionMatrix() const override;
	virtual glm::mat4 GetViewMatrix() const override;
	virtual glm::quat GetRotation() const override;
	virtual glm::vec3 GetDirection() const override;
	virtual glm::vec3 GetPosition() const override;

	virtual Physics::Primitives::C_Frustum GetFrustum()	const override;


	virtual float GetAspectRatio() const;

	void setupCameraView(float zoom, glm::vec3 center, float angleXDeg, float angleYDeg);
	void setCenterPoint(const glm::vec3& center);
	void setupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovYDeg);
	void adjustZoom(int zoom);

	void adjustOrientation(float dx, float dy);

	void update();


	virtual float GetFar() const { return _farZ; }
	virtual float GetNear() const { return _nearZ; }

	virtual float GetFov() const;


	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

	//virtual bool Input(SDL_Event) override;
protected:
	bool OnKeyPressed(Core::C_KeyPressedEvent& event);
private:
	glm::vec3 _pos;
	glm::vec3 _view;
	glm::vec3 _left;
	glm::vec3 _up;
	glm::vec3 _center;

	float _fovy;
	float _nearZ;
	float _farZ;
	float _aspect;

	float _zoom;
	float _angleXDeg;
	float _angleYDeg;

	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;

	float m_ControlSpeed;
};
}}}