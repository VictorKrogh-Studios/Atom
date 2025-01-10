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

		m_Window = Window::Create();
	}

	Application::~Application()
	{
		delete m_Window;
		m_Window = nullptr;

		Log::Shutdown();
	}

	void Application::Run()
	{
		m_Window->Update();
	}

}