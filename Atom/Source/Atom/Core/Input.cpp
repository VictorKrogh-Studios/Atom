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

	bool Input::IsMouseButtonPressed(Enumerations::MouseButton button)
	{
		int32_t state = glfwGetMouseButton(Application::Get().GetWindow()->m_WindowHandle, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	bool Input::IsMouseButtonReleased(Enumerations::MouseButton button)
	{
		int32_t state = glfwGetMouseButton(Application::Get().GetWindow()->m_WindowHandle, static_cast<int32_t>(button));
		return state == GLFW_RELEASE;
	}

	glm::vec2 Input::GetMousePosition()
	{
		double xpos, ypos;
		glfwGetCursorPos(Application::Get().GetWindow()->m_WindowHandle, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

}