#include "ATPCH.h"
#include "Application.h"

#include "Atom/Core/Timer.h"
#include "Atom/Core/Layer.h"

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
		for (Layer* layer : m_LayerStack)
		{
			layer->OnDetach();
			delete layer;
		}

		delete m_Window;
		m_Window = nullptr;

		Log::Shutdown();
	}

	void Application::Run()
	{
		Timer timer;

		while (m_IsRunning)
		{
			m_Window->Update();

			m_Window->AquireNextImage();

			float time = timer.Elapsed();
			m_DeltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate(m_DeltaTime);
			}

			m_Window->Present();
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
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