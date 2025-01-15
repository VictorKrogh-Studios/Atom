#pragma once

#include <vulkan/vulkan.h>

namespace Atom::Internal
{

	class VulkanBuffer
	{
	protected:
		void CreateBuffer(VkDevice device, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
		void CopyBuffer(VkDevice device, VkBuffer srcBuffer, VkBuffer dstBuffer, uint64_t size) const;
	};

}
