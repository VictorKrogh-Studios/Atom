#include "ATPCH.h"
#include "Window.h"

#include "Atom/Events/WindowEvent.h"

#include <GLFW/glfw3.h>

namespace Atom
{

	static void GLFWErrorCallback(int error, const char* description)
	{
		AT_CORE_ERROR("[GLFW] GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const WindowOptions& options)
	{
		return new Window(options);
	}

	Window::Window(const WindowOptions& options)
		: m_Options(options)
	{
		m_Data.Title = m_Options.Title;
		m_Data.Width = m_Options.Width;
		m_Data.Height = m_Options.Height;

		AT_CORE_ASSERT(glfwInit(), "Failed to initialize GLFW.");

		glfwSetErrorCallback(GLFWErrorCallback);

		SetClientAPI();

		m_WindowHandle = CreateWindowHandle();
		AT_CORE_ASSERT(m_WindowHandle, "Failed to create GLFW window.");

		//glfwMakeContextCurrent(m_WindowHandle);

		InitializeGraphicsContext();

		InitializeSwapChain();

		glfwSetWindowUserPointer(m_WindowHandle, &m_Data);

		glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* window)
		{
			auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow* window, int32_t newWidth, int32_t newHeight)
		{
			auto& data = *((WindowData*)glfwGetWindowUserPointer(window));

			WindowResizeEvent event((uint32_t)newWidth, (uint32_t)newHeight);

			data.Width = (uint32_t)newWidth;
			data.Height = (uint32_t)newHeight;

			data.EventCallback(event);
		});
	}

	Window::~Window()
	{
		delete m_SwapChain;
		m_SwapChain = nullptr;

		delete m_GraphicsContext;
		m_GraphicsContext = nullptr;

		glfwDestroyWindow(m_WindowHandle);
		glfwTerminate();
	}

	void Window::ResizeSwapChain(uint32_t width, uint32_t height)
	{
		m_SwapChain->Resize(width, height);
	}

	void Window::Update()
	{
		glfwPollEvents();
		//glfwSwapBuffers(m_WindowHandle);
	}

	void Window::InitializeGraphicsContext()
	{
		AT_CORE_ASSERT(glfwVulkanSupported(), "GLFW must support Vulkan!");

		uint32_t count = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&count);

		GraphicsContextOptions options{};
		options.GlfwExtensionsCount = count;
		options.GlfwExtensions = extensions;

		m_GraphicsContext = GraphicsContext::Create(options);
	}

	void Window::InitializeSwapChain()
	{
		SwapChainOptions swapChainOptions{};
		swapChainOptions.WindowHandle = m_WindowHandle;
		swapChainOptions.Width = m_Data.Width;
		swapChainOptions.Height = m_Data.Height;
		swapChainOptions.VSync = false;
		swapChainOptions.FramesInFlight = m_Options.FramesInFlight;

		m_SwapChain = SwapChain::Create(swapChainOptions);
	}

	GLFWwindow* Window::CreateWindowHandle()
	{
		bool fullscreen = false;
		if (fullscreen)
		{
			AT_CORE_ASSERT(!fullscreen, "Fullscreen support is not yet supported!");
		}

		return glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);
	}

	void Window::SetClientAPI()
	{
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	}

}