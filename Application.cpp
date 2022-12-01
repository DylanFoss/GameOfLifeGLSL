#include "Application.h"

#include <chrono>

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name, uint32_t width, uint32_t height)
{
	if (!s_Instance)
		s_Instance = this;

	m_Window = std::unique_ptr<Window>(Window::Create({ name, width, height }));
	m_Window->SetEventCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
}

int Application::Run()
{
	while (m_Running)
	{

		auto start = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch());
		float time = (float)start.count();
		TimeStep timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;

		Update(time);
		Draw(time);

		m_Window->OnUpdate();
	}

	return 0;
}

void Application::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Application::OnWindowClose, this, std::placeholders::_1));
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}
