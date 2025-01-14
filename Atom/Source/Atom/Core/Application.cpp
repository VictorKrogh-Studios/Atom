#include "ATPCH.h"
#include "Application.h"

#include "Atom/Core/Timer.h"
#include "Atom/Core/Layer.h"

#include "Atom/Graphics/Renderer.h"

namespace Atom
{

	Application::Application(const ApplicationCreateInfo& applicationCreateInfo)
		: m_CreateInfo(applicationCreateInfo)
	{
		Log::Initialize();

		WindowOptions windowOptions{};
		windowOptions.FramesInFlight = m_CreateInfo.FramesInFlight;

		m_Window = Window::Create(windowOptions);
		m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });


		RendererInitializeInfo rendererInitializeInfo{};
		rendererInitializeInfo.FramesInFlight = m_CreateInfo.FramesInFlight;

		Renderer::Initialize(rendererInitializeInfo);
	}

	Application::~Application()
	{
		Renderer::Shutdown();

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

			Renderer::BeginFrame();

			float time = timer.Elapsed();
			m_DeltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate(m_DeltaTime);
			}

			Renderer::EndFrame();

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