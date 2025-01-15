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
		VulkanCommandBuffer(uint32_t count);
		~VulkanCommandBuffer();
	private:
		void CreateCommandPool(VkDevice device, Internal::VulkanPhysicalDevice* physicalDevice);
		void CreateCommandBuffer(VkDevice device);
	private:
		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		friend class VulkanRenderCommand;
	};

}
