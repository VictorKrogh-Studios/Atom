#pragma once
#include "Atom/Graphics/Enumerations/ImageFormat.h"

#include "Atom/Graphics/Texture.h"

namespace Atom
{

	class RenderPass;

	struct FramebufferCreateInfo
	{
		uint32_t Width;
		uint32_t Height;
		RenderPass* RenderPass;
		bool TargetSwapChain = false; // True = target swapchain image(s), False = creates own image(s)
	};

	class Framebuffer
	{
	public:
		static Framebuffer* Create(const FramebufferCreateInfo& createInfo);
	public:
		Framebuffer(const FramebufferCreateInfo& createInfo);
		virtual ~Framebuffer() = default;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual RenderTexture* GetColorAttachment(uint32_t index) const = 0;
	protected:
		FramebufferCreateInfo m_CreateInfo;
	};

}
