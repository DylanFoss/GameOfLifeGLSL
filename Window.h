#pragma once
#include <string>
#include "GLFW/glfw3.h"


struct WindowData
{
	std::string Title;
	uint32_t Width;
	uint32_t Height;
	bool vsync = false;

	WindowData(const std::string& title = "Test Application", uint32_t width = 1280, uint32_t height = 720)
		:Title(title), Width(width), Height(height)
	{
	}
};

class Window
{
public:
	Window(const WindowData& data);
	~Window();

	void OnUpdate();

	inline uint32_t GetWidth() const { return m_Data.Width; }
	inline uint32_t GetHeight() const { return m_Data.Height; }

	void SetVsync(bool enable);
	bool IsVsync() const;

	static Window* Create(const WindowData& data = WindowData());

private:
	void Init(const WindowData& data);
	void Shutdown();

	WindowData m_Data;
	GLFWwindow* m_Window;
	bool glfwInitialised = false;
};