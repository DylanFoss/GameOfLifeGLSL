#include "Window.h"
#include "WindowEvent.h"

Window::Window(const WindowProps& data)
{
    Init(data);
}

Window::~Window()
{
    Shutdown();
}

void Window::Shutdown()
{
    glfwDestroyWindow(m_Window);
}

void Window::Init(const WindowProps& data)
{
    m_Data.Title = data.Title;
    m_Data.Height = data.Height;
    m_Data.Width = data.Width;

    if (!glfwInitialised)
    {
        int initialised = glfwInit();
        if (initialised)
            glfwInitialised = true;
    }

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
    if (!m_Window)
    {
        glfwTerminate();
        glfwInitialised = false;
    }

    //
    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, &m_Data);
    SetVsync(true);

    //set resize / input callbacks here

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
	{
		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		WindowCloseEvent event;
		data.EventCallback(event);
	});

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
    {
        WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

        data.Width = width;
        data.Height = height;

        WindowResizeEvent event(width, height);
        data.EventCallback(event);
    });

}

void Window::OnUpdate()
{
    /* Swap front and back buffers */
    glfwSwapBuffers(m_Window);
    /* Poll for and process events */
    glfwPollEvents();
}

void Window::SetVsync(bool enable)
{
    if (enable)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);

    m_Data.vsync = enable;
}

bool Window::IsVsync() const
{
    return m_Data.vsync;
}

Window* Window::Create(const WindowProps& data)
{
    return new Window(data);
}
