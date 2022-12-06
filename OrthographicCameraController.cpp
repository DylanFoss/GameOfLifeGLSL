#include "OrthographicCameraController.h"

#include "Input.h"
#include "KeyCodes.h"

Utils::OrthographicCameraController::OrthographicCameraController(float aspectRatio)
	:m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_Zoom, m_AspectRatio* m_Zoom, -m_Zoom, m_Zoom)
{
}

void Utils::OrthographicCameraController::Update(float deltaTime)
{
	if (Input::Get().IsKeyPressed(KC_KEY_A))
	{
		m_CameraPosition.x -= cos(glm::radians(0.0f)) * m_CameraTranslationSpeed * deltaTime;
		m_CameraPosition.y -= sin(glm::radians(0.0f)) * m_CameraTranslationSpeed * deltaTime;
	}
	else if  (Input::Get().IsKeyPressed(KC_KEY_D))
	{
		m_CameraPosition.x += cos(glm::radians(0.0f)) * m_CameraTranslationSpeed * deltaTime;
		m_CameraPosition.y += sin(glm::radians(0.0f)) * m_CameraTranslationSpeed * deltaTime;
	}

	if (Input::Get().IsKeyPressed(KC_KEY_W))
	{
		m_CameraPosition.x += -sin(glm::radians(0.0f)) * m_CameraTranslationSpeed * deltaTime;
		m_CameraPosition.y += cos(glm::radians(0.0f)) * m_CameraTranslationSpeed * deltaTime;
	}
	else if (Input::Get().IsKeyPressed(KC_KEY_S))
	{
		m_CameraPosition.x -= -sin(glm::radians(0.0f)) * m_CameraTranslationSpeed * deltaTime;
		m_CameraPosition.y -= cos(glm::radians(0.0f)) * m_CameraTranslationSpeed * deltaTime;
	}

	if (Input::Get().GetMouseScroll() != 0)
	{
		m_Zoom -= Input::Get().GetMouseScroll() * 0.25f;
		m_Zoom = std::max(m_Zoom, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
	}

	m_Camera.SetPosition(m_CameraPosition);
	m_CameraTranslationSpeed = m_Zoom;
}
