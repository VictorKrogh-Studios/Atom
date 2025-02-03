#pragma once
#include "Atom/Graphics/RenderPass.h"
#include "Atom/Graphics/Framebuffer.h"

#include "Atom/Graphics/Vulkan/VulkanSwapChain.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(const RenderPassCreateInfo& renderPassCreateInfo);
		~VulkanRenderPass();

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual void SetRenderTarget(Framebuffer* framebuffer) override;

		inline VkRenderPass GetVkRenderPass() const
		{
			return m_RenderPass;
		}

		VkFramebuffer GetVkFramebuffer() const;

		inline VkClearValue GetVkClearValue() const
		{
			return { m_CreateInfo.ClearColor.r, m_CreateInfo.ClearColor.g, m_CreateInfo.ClearColor.b, m_CreateInfo.ClearColor.a };
		}

		inline VkExtent2D GetRenderAreaExtent() const
		{
			return m_RenderArea;
			//VkExtent2D extent{};
			//extent.width = m_CreateInfo.RenderArea.x;
			//extent.height = m_CreateInfo.RenderArea.y;
			//return extent;
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
		void CreateRenderPass(VkDevice device);
	private:
		Framebuffer* m_Framebuffer = nullptr;

		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		//VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;

		VkExtent2D m_RenderArea;

		friend class VulkanPipeline;
		friend class VulkanRenderCommand;
	};

}
