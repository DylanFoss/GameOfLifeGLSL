#include "pch.h"
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
	std::chrono::time_point <std::chrono::high_resolution_clock> t1, t2;
	t1 = std::chrono::high_resolution_clock::now();
	while (m_Running)
	{
		t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsedTime = t2 - t1;
		float deltaTime = elapsedTime.count();

		Update(deltaTime);
		Draw(deltaTime);

		m_Window->OnUpdate();
		t1 = t2;
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
