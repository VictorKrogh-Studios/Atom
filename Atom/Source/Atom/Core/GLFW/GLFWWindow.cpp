#include "ATPCH.h"
#include "GLFWWindow.h"

#include <GLFW/glfw3.h>

namespace Atom
{

	GLFWWindow::GLFWWindow()
	{
		if (glfwInit() == GLFW_FALSE)
		{
			AT_CORE_ERROR("Failed to initialize GLFW.");
			__debugbreak();
		}
	}

	GLFWWindow::~GLFWWindow()
	{
	}

}