#pragma once
#include "Atom/Core/Window.h"

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
		Window* m_Window = nullptr;
	};

	// Implemented by CLIENT
	Application* CreateApplication(int argc, char** argv);

}