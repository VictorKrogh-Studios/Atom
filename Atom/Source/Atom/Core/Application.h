#pragma once

#include "Window.h"

namespace Atom
{

	class Application
	{
	public:
		Application();
		~Application();
	public:
		void Run();
	private:
		Window* m_Window;
	};

	// Implemented by CLIENT
	Application* CreateApplication(int argc, char** argv);

}