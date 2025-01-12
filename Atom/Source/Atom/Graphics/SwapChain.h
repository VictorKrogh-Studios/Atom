#pragma once

struct GLFWwindow;

namespace Atom
{

	struct SwapChainOptions
	{
		uint32_t Width;
		uint32_t Height;
		bool VSync;

		GLFWwindow* WindowHandle;
	};

	class SwapChain
	{
	public:
		static SwapChain* Create(const SwapChainOptions& options);
	public:
		SwapChain(const SwapChainOptions& options);
		virtual ~SwapChain() = default;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void Present() = 0;
	protected:
		SwapChainOptions m_Options;
	};

}
