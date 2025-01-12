#pragma once
#include "Atom/Core/Window.h"
#include "Atom/Core/LayerStack.h"

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
	protected:
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
	private:
		void OnEvent(Event& event);
		bool OnWindowCloseEvent(WindowCloseEvent& event);
	private:
		bool m_IsRunning = true;
		Window* m_Window = nullptr;
		LayerStack m_LayerStack;
	};

	// Implemented by CLIENT
	Application* CreateApplication(int argc, char** argv);

}