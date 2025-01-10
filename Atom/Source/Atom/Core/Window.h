#pragma once
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
		static Window* Create(const WindowOptions& options);
	public:
		Window(const WindowOptions& options);
		~Window();

		void InitializeGraphicsContext();
		void InitializeSwapChain();

		void Update();
	private:
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
		} m_Data;
	};

}