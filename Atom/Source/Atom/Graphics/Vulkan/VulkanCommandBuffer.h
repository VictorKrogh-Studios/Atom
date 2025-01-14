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
		VulkanCommandBuffer();
		~VulkanCommandBuffer();
	private:
		void CreateCommandPool(VkDevice device, Internal::VulkanPhysicalDevice* physicalDevice);
		void CreateCommandBuffer(VkDevice device);
	private:
		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;

		friend class VulkanRenderCommand;
	};

}
