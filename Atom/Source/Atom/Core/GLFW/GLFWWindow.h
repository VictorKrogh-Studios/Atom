#pragma once
#include "Atom/Core/Window.h"

namespace Atom
{

	class GLFWWindow : public Window
	{
	public:
		GLFWWindow();
		virtual ~GLFWWindow();
	};

}