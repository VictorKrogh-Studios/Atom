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
		s_Instance = this;

		Log::Initialize();

		WindowOptions windowOptions{};
		windowOptions.FramesInFlight = m_CreateInfo.FramesInFlight;

		m_Window = Window::Create(windowOptions);
		m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });


		RendererInitializeInfo rendererInitializeInfo{};
		rendererInitializeInfo.SwapChain = m_Window->GetSwapChain();
		rendererInitializeInfo.FramesInFlight = m_CreateInfo.FramesInFlight;

		Renderer::Initialize(rendererInitializeInfo);

		m_ImGuiLayer = ImGuiLayer::Create(m_Window, Renderer::GetRenderCommand());
		PushOverlay(m_ImGuiLayer);
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

			float time = timer.Elapsed();
			m_DeltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			Renderer::BeginFrame();

			for (Layer* layer : m_LayerStack)
			{
				layer->OnUpdate(m_DeltaTime);
			}

			m_ImGuiLayer->Begin();
			{
				for (Layer* layer : m_LayerStack)
				{
					layer->OnImGui();
				}
			}
			m_ImGuiLayer->End();

			Renderer::EndFrame();

			Renderer::PresentAndWait();
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