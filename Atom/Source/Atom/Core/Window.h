#pragma once
#include "Atom/Events/Event.h"

#include "Atom/Graphics/GraphicsContext.h"

#include <string>

struct GLFWwindow;

namespace Atom
{
	
	struct WindowOptions
	{
		std::string Title = "Atom";
		uint32_t Width = 1600;
		uint32_t Height = 900;
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

		void InitializeSwapChain();

		void SetEventCallback(const EventCallbackFn& callback) 
		{
			m_Data.EventCallback = callback;
		}

		void Update();
	private:
		void InitializeGraphicsContext();

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
	};

}