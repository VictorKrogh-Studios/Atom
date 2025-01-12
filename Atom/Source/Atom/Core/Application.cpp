#include "ATPCH.h"
#include "Application.h"

namespace Atom
{

	Application::Application()
	{
		Log::Initialize();

		m_Window = Window::Create({});
		m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });
	}

	Application::~Application()
	{
		delete m_Window;
		m_Window = nullptr;

		Log::Shutdown();
	}

	void Application::Run()
	{
		while (m_IsRunning)
		{
			m_Window->Update();
		}
	}

	void Application::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent& e) { return OnWindowCloseEvent(e); });
	}

	bool Application::OnWindowCloseEvent(WindowCloseEvent& event)
	{
		m_IsRunning = false;
		return true;
	}

}