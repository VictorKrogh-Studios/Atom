#pragma once
#include "Atom/Core/Window.h"
#include "Atom/Core/LayerStack.h"

#include "Atom/Events/WindowEvent.h"

namespace Atom
{

	struct CommandLineArgs
	{
		int32_t Count;
		char** Args;
	};

	struct ApplicationCreateInfo
	{
		CommandLineArgs Args;
		uint32_t FramesInFlight = 3;
	};

	class ImGuiLayer;

	class Application
	{
	public:
		Application(const ApplicationCreateInfo& applicationCreateInfo);
		~Application();
	public:
		void Run();

		void Close();

		inline static void SubmitReleaseQueue(const std::function<void()> func)
		{
			s_Instance->m_ResourceReleaseQueue.push_back(func);
		}

		static Application& Get() { return *s_Instance; };
		Window* GetWindow() const { return m_Window; }
	protected:
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);
	private:
		void OnEvent(Event& event);
		bool OnWindowCloseEvent(WindowCloseEvent& event);
		bool OnWindowResizeEvent(WindowResizeEvent& event);
	private:
		ApplicationCreateInfo m_CreateInfo;
		bool m_IsRunning = true;
		Window* m_Window = nullptr;
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer = nullptr;

		float m_LastFrameTime = 0.0f;
		float m_DeltaTime = 0.0f;

		std::vector<std::function<void()>> m_ResourceReleaseQueue;
	private:
		inline static Application* s_Instance = nullptr;
	};

	// Implemented by CLIENT
	Application* CreateApplication(const CommandLineArgs& args);

}