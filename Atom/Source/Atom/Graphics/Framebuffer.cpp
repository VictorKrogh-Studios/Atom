#include "ATPCH.h"
#include "Atom/Graphics/Framebuffer.h"

#include "Atom/Graphics/Vulkan/VulkanFramebuffer.h"

namespace Atom
{

	Framebuffer* Framebuffer::Create(const FramebufferCreateInfo& createInfo)
	{
		return new VulkanFramebuffer(createInfo);
	}

	Framebuffer::Framebuffer(const FramebufferCreateInfo& createInfo)
		: m_CreateInfo(createInfo), m_Width(createInfo.Width), m_Height(createInfo.Height)
	{
	}

}