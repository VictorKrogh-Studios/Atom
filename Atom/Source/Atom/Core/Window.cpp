#include "ATPCH.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace Atom
{

	Window* Window::Create()
	{
		return new Window();
	}

	Window::Window()
	{
		AT_CORE_ASSERT(glfwInit(), "Failed to initialize GLFW.");

		m_WindowHandle = glfwCreateWindow(1280, 720, "Hello, world!", nullptr, nullptr);
		AT_CORE_ASSERT(m_WindowHandle, "Failed to create GLFW window.");

		glfwMakeContextCurrent(m_WindowHandle);
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_WindowHandle);
	}

	void Window::Update()
	{
		while (glfwWindowShouldClose(m_WindowHandle) == GLFW_FALSE)
		{
			glfwPollEvents();
			glfwSwapBuffers(m_WindowHandle);
		}
	}

}