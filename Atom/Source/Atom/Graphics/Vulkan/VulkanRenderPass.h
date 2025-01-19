#pragma once
#include "Atom/Graphics/RenderPass.h"

#include "Atom/Graphics/Vulkan/VulkanSwapChain.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(const RenderPassCreateInfo& renderPassCreateInfo);
		~VulkanRenderPass();

		inline VkRenderPass GetVkRenderPass() const
		{
			return m_RenderPass;
		}

		inline VkFramebuffer GetVkFramebuffer() const
		{
			if (m_CreateInfo.TargetSwapChain)
			{
				return VulkanSwapChain::Get()->GetCurrentFramebuffer();
			}

			AT_CORE_ASSERT(true, "Render passes only support targeting to the Swap Chain framebuffer");
			return VK_NULL_HANDLE;
		}

		inline VkClearValue GetVkClearValue() const
		{
			return GetClearColor();
		}

		inline VkExtent2D GetRenderAreaExtent() const
		{
			return GetRenderArea();
		}
	private:
		void CreateRenderPass(VkDevice device);

		inline VkClearValue GetClearColor() const
		{
			return { m_CreateInfo.ClearColor.r, m_CreateInfo.ClearColor.g, m_CreateInfo.ClearColor.b, m_CreateInfo.ClearColor.a };
		}

		inline VkExtent2D GetRenderArea() const
		{
			VkExtent2D extent{};
			extent.width = m_CreateInfo.RenderArea.x;
			extent.height = m_CreateInfo.RenderArea.y;
			return extent;
		}

		inline VkSubpassContents GetVkSubpassContents()
		{
			switch (m_CreateInfo.SubpassContents)
			{
				case Atom::Enumerations::RenderPassSubpassContents::Inline: return VK_SUBPASS_CONTENTS_INLINE;
				case Atom::Enumerations::RenderPassSubpassContents::SecondaryCommandBuffer: return VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;
				default: break;
			}

			AT_CORE_ASSERT(true, "Unknown subpass contents");
			return VK_SUBPASS_CONTENTS_MAX_ENUM;
		}
	private:
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		friend class VulkanPipeline;
		friend class VulkanRenderCommand;
	};

}
