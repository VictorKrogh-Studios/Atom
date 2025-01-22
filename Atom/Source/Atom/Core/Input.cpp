#include "ATPCH.h"
#include "Atom/Core/Input.h"

#include "Atom/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Atom
{

	bool Input::IsKeyPressed(Enumerations::KeyCode keyCode)
	{
		int32_t state = glfwGetKey(Application::Get().GetWindow()->m_WindowHandle, static_cast<int32_t>(keyCode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsKeyReleased(Enumerations::KeyCode keyCode)
	{
		int32_t state = glfwGetKey(Application::Get().GetWindow()->m_WindowHandle, static_cast<int32_t>(keyCode));
		return state == GLFW_RELEASE;
	}

}