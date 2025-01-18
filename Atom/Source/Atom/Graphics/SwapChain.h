#pragma once
#include "Atom/Graphics/Enumerations/ImageFormat.h"

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

		uint32_t GetWidth() const { return m_Options.Width; }
		uint32_t GetHeight() const { return m_Options.Height; }
		Enumerations::ImageFormat GetImageFormat() const { return m_ImageFormat; }
	protected:
		SwapChainOptions m_Options;
		Enumerations::ImageFormat m_ImageFormat;
	};

}
