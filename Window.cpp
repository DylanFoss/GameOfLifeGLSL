#include "Window.h"

Window::Window(const WindowData& data)
{
    Init(data);
}

Window::~Window()
{
    Shutdown();
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

Window* Window::Create(const WindowData& data)
{
    return new Window(data);
}

void Window::Init(const WindowData& data)
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


    m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
    if (!m_Window)
    {
        glfwTerminate();
        glfwInitialised = false;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(true); // 1 to cap to refresh rate

    //set resize / input callbacks here
    /*
    */
}

void Window::Shutdown()
{
    glfwDestroyWindow(m_Window);
}
