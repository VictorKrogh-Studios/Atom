#include "ATPCH.h"
#include "GLFWWindow.h"

#include <GLFW/glfw3.h>

namespace Atom
{

	GLFWWindow::GLFWWindow()
	{
		AT_CORE_ASSERT(glfwInit(), "Failed to initialize GLFW.");

		m_WindowHandle = glfwCreateWindow(1280, 720, "Hello, world!", nullptr, nullptr);
		AT_CORE_ASSERT(m_WindowHandle, "Failed to create GLFW window.");

		glfwMakeContextCurrent(m_WindowHandle);

		// Create swapchain
	}

	GLFWWindow::~GLFWWindow()
	{
		glfwDestroyWindow(m_WindowHandle);
	}

	void GLFWWindow::Update()
	{
		while (glfwWindowShouldClose(m_WindowHandle) == GLFW_FALSE)
		{
			glfwPollEvents();
			glfwSwapBuffers(m_WindowHandle);
		}
	}

}