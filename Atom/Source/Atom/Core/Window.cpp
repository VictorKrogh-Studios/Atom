#include "ATPCH.h"
#include "Window.h"

#include "Atom/Events/WindowEvent.h"

#include <GLFW/glfw3.h>

namespace Atom
{

	static void GLFWErrorCallback(int error, const char* description)
	{
		AT_CORE_ERROR("[GLFW] GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowOptions& options)
	{
		return new Window(options);
	}

	Window::Window(const WindowOptions& options)
		: m_Options(options)
	{
		m_Data.Title = m_Options.Title;
		m_Data.Width = m_Options.Width;
		m_Data.Height = m_Options.Height;

		AT_CORE_ASSERT(glfwInit(), "Failed to initialize GLFW.");

		glfwSetErrorCallback(GLFWErrorCallback);

		SetClientAPI();

		m_WindowHandle = CreateWindowHandle();
		AT_CORE_ASSERT(m_WindowHandle, "Failed to create GLFW window.");

		glfwSetWindowUserPointer(m_WindowHandle, &m_Data);

		//glfwMakeContextCurrent(m_WindowHandle);

		glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* window)
		{
			auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

			WindowCloseEvent event;
			data.EventCallback(event);
		});
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_WindowHandle);
		glfwTerminate();
	}

	void Window::InitializeGraphicsContext()
	{
		// TODO: Implement
	}

	void Window::InitializeSwapChain()
	{
		// TODO: Implement
	}

	void Window::Update()
	{
		glfwPollEvents();
		//glfwSwapBuffers(m_WindowHandle);
	}

	GLFWwindow* Window::CreateWindowHandle()
	{
		bool fullscreen = false;
		if (fullscreen)
		{
			AT_CORE_ASSERT(!fullscreen, "Fullscreen support is not yet supported!");
		}

		return glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
	}

	void Window::SetClientAPI()
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

}