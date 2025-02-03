#include "ATPCH.h"
#include "Atom/Core/Application.h"

#include "Atom/Core/Timer.h"
#include "Atom/Core/Layer.h"

#include "Atom/Graphics/Renderer.h"

#include "Atom/ImGui/ImGuiLayer.h"

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

		m_ImGuiLayer = ImGuiLayer::Create(m_Window);
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

			for (const auto& func : m_ResourceReleaseQueue)
			{
				func();
			}
			m_ResourceReleaseQueue.clear();
		}
	}

	void Application::Close()
	{
		m_IsRunning = false;
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
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e) { return OnWindowResizeEvent(e); });

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(event);
			if (event.Handled)
			{
				break;
			}
		}
	}

	bool Application::OnWindowCloseEvent(WindowCloseEvent& event)
	{
		m_IsRunning = false;
		return true;
	}

	bool Application::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		m_Window->ResizeSwapChain(event.GetWidth(), event.GetHeight());
		return false;
	}

}