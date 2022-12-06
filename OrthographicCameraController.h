#pragma once
#include "OrthographicCamera.h"

namespace Utils {

class OrthographicCameraController
{
public:
	OrthographicCameraController(float aspectRatio);

	void Update(float deltaTime);

	OrthographicCamera& GetCamera() { return m_Camera; }

	float GetZoom() const { return m_Zoom; }
	void SetZoom(float zoom) { m_Zoom = zoom; }

private:

	float m_AspectRatio;
	float m_Zoom = 1.0f;

	OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
	float m_CameraTranslationSpeed = 5.0f;

};

}

