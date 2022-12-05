#include "Input.h"
#include "Application.h"
#include "GLFW/glfw3.h"

Input* Input::s_Instance = nullptr;

Input::Input()
{
	if (!s_Instance)
		s_Instance = this;

	std::memset(m_KeyStates, 0, sizeof(bool) * s_numKeys);
	std::memset(m_KeyStatesOld, 0, sizeof(bool) * s_numKeys);

	std::memset(m_MouseStates, 0, sizeof(bool) * s_numMouseKeys);
	std::memset(m_MouseStatesOld, 0, sizeof(bool) * s_numMouseKeys);
}

bool Input::IsKeyPressed(int key) const
{
	return m_KeyStates[key] && !m_KeyStatesOld[key];
}

bool Input::IsKeyHeld(int key) const
{
	return m_KeyStates[key] && m_KeyStatesOld[key];
}

bool Input::IsMousePressed(int button) const
{
	return m_MouseStates[button] && !m_MouseStatesOld[button];
}

bool Input::IsMouseHeld(int button) const
{
	return m_MouseStates[button] && m_MouseStatesOld[button];
}

std::pair<int, int> Input::GetMousePos(int button) const
{
	return std::pair<int, int>();
}

void Input::KeysDown(int key)
{
	std::cout << (char)key << " was pressed.\n";
	m_KeyStates[key] = true;
}

void Input::KeysUp(int key)
{
	std::cout << (char)key << " was released.\n";
	m_KeyStates[key] = false;
}

void Input::KeysHeld()
{
	for (int i = 0; i < s_numKeys; i++)
		m_KeyStatesOld[i] = m_KeyStates[i];
}

void Input::MouseButtonsDown(int button)
{
	std::cout << "Mouse " << button << " was pressed.\n";
	m_MouseStates[button] = true;
}

void Input::MouseButtonsUp(int button)
{
	std::cout << "Mouse " << button << " was released.\n";
	m_MouseStatesOld[button] = false;
}

void Input::MouseButtonsHeld()
{
	for (int i = 0; i < s_numMouseKeys; i++)
		m_KeyStatesOld[i] = m_KeyStates[i];
}

void Input::MousePosition(int x, int y)
{
	std::cout << "Mouse Pos: " << x << ' ' << y << '\n';
	m_MousePosX = x;
	m_MousePosY = y;
}

