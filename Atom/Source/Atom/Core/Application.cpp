#include "ATPCH.h"
#include "Application.h"

namespace Atom
{

	Application::Application()
	{
		Log::Initialize();

		AT_CORE_TRACE("TRACE");
		AT_CORE_INFO("INFO");
		AT_CORE_WARN("WARNING");
		AT_CORE_ERROR("ERROR");
		AT_CORE_CRITICAL("CRITICAL");

		m_Window = Window::Create({});
		m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });
		m_Window->InitializeGraphicsContext();
		m_Window->InitializeSwapChain();
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