#include "ATPCH.h"
#include "Window.h"

#include "Atom/Core/GLFW/GLFWWindow.h"

namespace Atom
{

	Window* Window::Create()
	{
		return new GLFWWindow();
	}

}