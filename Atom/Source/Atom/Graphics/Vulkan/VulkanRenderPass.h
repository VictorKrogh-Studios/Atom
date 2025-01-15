#pragma once
#include "Atom/Graphics/RenderPass.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(const RenderPassCreateInfo& renderPassCreateInfo);
		~VulkanRenderPass();
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
	private:
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		friend class VulkanPipeline;
		friend class VulkanRenderCommand;
	};

}
