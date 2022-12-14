#include "OrthographicCameraController.h"

#include "Input.h"
#include "KeyCodes.h"
#include "MouseCodes.h"

Utils::OrthographicCameraController::OrthographicCameraController(float orthoX, float orthoY)
	:m_OrthoX(orthoX), m_OrthoY(orthoY), m_Camera(-orthoX * m_Zoom, orthoX* m_Zoom, -orthoY * m_Zoom, orthoY* m_Zoom)
{
}

void Utils::OrthographicCameraController::Update(float deltaTime)
{
	if (Input::Get().IsKeyHeld(KC_KEY_A))
	{
		m_CameraPosition.x -= cos(glm::radians(0.0f)) * m_CameraTranslationSpeed;
		m_CameraPosition.y -= sin(glm::radians(0.0f)) * m_CameraTranslationSpeed;
	}
	else if  (Input::Get().IsKeyHeld(KC_KEY_D))
	{
		m_CameraPosition.x += cos(glm::radians(0.0f)) * m_CameraTranslationSpeed;
		m_CameraPosition.y += sin(glm::radians(0.0f)) * m_CameraTranslationSpeed;
	}

	if (Input::Get().IsKeyHeld(KC_KEY_W))
	{
		m_CameraPosition.x += -sin(glm::radians(0.0f)) * m_CameraTranslationSpeed;
		m_CameraPosition.y += cos(glm::radians(0.0f)) * m_CameraTranslationSpeed;
	}
	else if (Input::Get().IsKeyHeld(KC_KEY_S))
	{
		m_CameraPosition.x -= -sin(glm::radians(0.0f)) * m_CameraTranslationSpeed;
		m_CameraPosition.y -= cos(glm::radians(0.0f)) * m_CameraTranslationSpeed;
	}

	if (Input::Get().IsMousePressed(KC_MOUSE_BUTTON_LEFT))
	{
		std::pair<int, int> pair = Input::Get().GetMousePos();
		glm::vec2 cursorPos = { pair.first, pair.second };

		glm::vec2 worldPos = ScreenToWorldSpace(cursorPos);

		std::cout << worldPos.x << ", " << worldPos.y << '\n';
	}

	if (Input::Get().GetMouseScroll() != 0)
	{
		m_Zoom -= Input::Get().GetMouseScroll() * 0.1;
		m_Zoom = std::max(m_Zoom, m_MaxZoom);
		m_Camera.SetProjection(-m_OrthoX * m_Zoom, m_OrthoX * m_Zoom, -m_OrthoY * m_Zoom, m_OrthoY * m_Zoom);
	}

	if (Input::Get().IsKeyPressed(KC_KEY_R))
	{
		m_CameraPosition.x = 0.0f;
		m_CameraPosition.y = 0.0f;
		m_Zoom = 1;
		m_Camera.SetProjection(-m_OrthoX * m_Zoom, m_OrthoX * m_Zoom, -m_OrthoY * m_Zoom, m_OrthoY * m_Zoom);
	}

	m_Camera.SetPosition(m_CameraPosition);
	m_CameraTranslationSpeed = m_Zoom;
}

glm::vec2 Utils::OrthographicCameraController::ScreenToWorldSpace(glm::vec2 screenPoint)
{
	return glm::vec2(
		((screenPoint.x - m_OrthoX)) * m_Zoom + m_Camera.GetPosition().x,
		((-screenPoint.y + m_OrthoY)) * m_Zoom + m_Camera.GetPosition().y
	);
}
