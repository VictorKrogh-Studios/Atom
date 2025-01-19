#pragma once
#include "Atom/Graphics/CommandBuffer.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	namespace Internal
	{

		class VulkanPhysicalDevice;

	}

	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(const CommandBufferCreateInfo& commandBufferCreateInfo, uint32_t count);
		virtual ~VulkanCommandBuffer();

		virtual void Reset(uint32_t index) const override;
		virtual void Begin(uint32_t index) const override;
		virtual void End(uint32_t index) const override;
		virtual void Submit(uint32_t index) const override;
	private:
		void CreateCommandBuffer(VkDevice device);
	private:
		std::vector<VkCommandBuffer> m_CommandBuffers;

		friend class VulkanRenderCommand;
		friend class VulkanImGuiLayer;
	};

}
