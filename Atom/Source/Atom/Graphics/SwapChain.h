#pragma once

struct GLFWwindow;

namespace Atom
{

	struct SwapChainOptions
	{
		uint32_t Width;
		uint32_t Height;
		bool VSync;
		uint32_t FramesInFlight;

		GLFWwindow* WindowHandle;
	};

	class SwapChain
	{
	public:
		static SwapChain* Create(const SwapChainOptions& options);
	public:
		SwapChain(const SwapChainOptions& options);
		virtual ~SwapChain() = default;

		virtual uint32_t AquireNextImage(uint32_t frameIndex) = 0;
		virtual void Present(uint32_t frameIndex, bool wait) const = 0;
	protected:
		SwapChainOptions m_Options;
	};

}
