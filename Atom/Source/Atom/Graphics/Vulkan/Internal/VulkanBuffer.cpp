#include "ATPCH.h"
#include "VulkanBuffer.h"

#include "Atom/Graphics/Vulkan/VulkanGraphicsContext.h"

namespace Atom::Internal
{

	void VulkanBuffer::CreateBuffer(VkDevice device, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const
	{
		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &buffer);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = VulkanGraphicsContext::GetPhysicalDevice()->FindMemoryType(memRequirements.memoryTypeBits, properties);

		result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &bufferMemory);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		result = vkBindBufferMemory(device, buffer, bufferMemory, 0);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanBuffer::CopyBuffer(VkDevice device, VkBuffer srcBuffer, VkBuffer dstBuffer, uint64_t size) const
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = VulkanGraphicsContext::Get()->m_TransferCommandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(VulkanGraphicsContext::GetDevice()->m_TransferQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(VulkanGraphicsContext::GetDevice()->m_TransferQueue);

		vkFreeCommandBuffers(device, VulkanGraphicsContext::Get()->m_TransferCommandPool, 1, &commandBuffer);
	}

}