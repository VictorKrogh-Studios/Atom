#pragma once
#include "Atom/Core/Window.h"

#include "Atom/Events/WindowEvent.h"

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
		void OnEvent(Event& event);
		bool OnWindowCloseEvent(WindowCloseEvent& event);
	private:
		bool m_IsRunning = true;
		Window* m_Window = nullptr;
	};

	// Implemented by CLIENT
	Application* CreateApplication(int argc, char** argv);

}