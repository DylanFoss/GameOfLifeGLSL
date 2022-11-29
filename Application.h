#pragma once
#include <string>
#include <memory>

#include "Window.h"
#include "Timestep.h"

class Application
{
public:
	Application(const std::string& name = "OpenGL Sandbox", uint32_t width = 1280, uint32_t height = 720);
	virtual ~Application() = default;

	int Run();

	virtual void Update(float deltaTime) = 0;
	virtual void Draw(float deltaTime) = 0;

private:

	std::unique_ptr<Window> m_Window;
	bool m_Running = true;
	float m_LastFrameTime = 0.0f;

	static Application* s_Instance;

};

