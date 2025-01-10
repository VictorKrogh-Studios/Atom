#pragma once
#include "Atom/Core/Window.h"

struct GLFWwindow;

namespace Atom
{

	class GLFWWindow : public Window
	{
	public:
		GLFWWindow();
		virtual ~GLFWWindow();

		virtual void Update() override;
	private:
		GLFWwindow* m_WindowHandle = nullptr;
	};

}