#pragma once
#include "Atom/Events/Event.h"

#include "Atom/Graphics/GraphicsContext.h"
#include "Atom/Graphics/SwapChain.h"

#include <string>

struct GLFWwindow;

namespace Atom
{
	
	struct WindowOptions
	{
		std::string Title = "Atom";
		uint32_t Width = 1600;
		uint32_t Height = 900;
		uint32_t FramesInFlight;
	};

	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
	public:
		static Window* Create(const WindowOptions& options);
	public:
		Window(const WindowOptions& options);
		~Window();

		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }

		void Update();

		SwapChain* GetSwapChain() const { return m_SwapChain; }
	private:
		void InitializeGraphicsContext();
		void InitializeSwapChain();

		GLFWwindow* CreateWindowHandle();
		void SetClientAPI();
	private:
		GLFWwindow* m_WindowHandle = nullptr;
		WindowOptions m_Options;

		struct WindowData
		{
			std::string Title;
			uint32_t Width;
			uint32_t Height;

			EventCallbackFn EventCallback;
		} m_Data;

		GraphicsContext* m_GraphicsContext = nullptr;
		SwapChain* m_SwapChain = nullptr;

		friend class ImGuiLayer;
	};

}