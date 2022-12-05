#pragma once
#include "pch.h"
class Input
{
public:
	Input();

	void KeysDown(int key);
	void KeysUp(int key);
	void KeysHeld();

	bool IsKeyPressed(int key) const;
	bool IsKeyHeld(int key) const;

	void MouseButtonsDown(int button);
	void MouseButtonsUp(int button);
	void MouseButtonsHeld();

	void MousePosition(int x, int y);

	bool IsMousePressed(int button) const;
	bool IsMouseHeld(int button) const;

	std::pair<int, int> GetMousePos(int button) const;

	inline static Input& Get() { return *s_Instance; }


private:

	static constexpr int s_numKeys = 256;
	static constexpr int s_numMouseKeys = 9;
	
	bool m_KeyStates[s_numKeys];
	bool m_KeyStatesOld[s_numKeys];


	bool m_MouseStates[s_numMouseKeys];
	bool m_MouseStatesOld[s_numMouseKeys];

	int m_MousePosX = 0;
	int m_MousePosY = 0;

	static Input* s_Instance;
};

