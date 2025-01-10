#pragma once
#include <memory>

struct GLFWwindow;

namespace Atom
{

	class Window
	{
	public:
		static Window* Create();
	public:
		Window();
		~Window();

		void Update();
	private:
		GLFWwindow* m_WindowHandle = nullptr;
	};

}