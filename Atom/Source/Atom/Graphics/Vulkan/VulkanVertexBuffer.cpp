#include "ATPCH.h"
#include "VulkanVertexBuffer.h"
#include "VulkanGraphicsContext.h"

namespace Atom
{

	VulkanVertexBuffer::VulkanVertexBuffer(uint64_t size, void* vertices)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
		CreateBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, size, 0, &data);
		memcpy(data, vertices, size);
		vkUnmapMemory(device, stagingBufferMemory);

		CreateBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

		CopyBuffer(device, stagingBuffer, m_Buffer, size);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		vkDestroyBuffer(device, m_Buffer, nullptr);
		vkFreeMemory(device, m_BufferMemory, nullptr);
	}

	void VulkanVertexBuffer::CreateBuffer(VkDevice device, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const
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

	void VulkanVertexBuffer::CopyBuffer(VkDevice device, VkBuffer srcBuffer, VkBuffer dstBuffer, uint64_t size) const
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