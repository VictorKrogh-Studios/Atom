#include "ATPCH.h"
#include "VulkanIndexBuffer.h"
#include "VulkanGraphicsContext.h"

namespace Atom
{

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t count, uint32_t* indices)
		: IndexBuffer(count)
	{
		uint64_t size = count * sizeof(uint32_t);

		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
		CreateBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, size, 0, &data);
		memcpy(data, indices, size);
		vkUnmapMemory(device, stagingBufferMemory);

		CreateBuffer(device, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

		CopyBuffer(device, stagingBuffer, m_Buffer, size);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		vkDestroyBuffer(device, m_Buffer, nullptr);
		vkFreeMemory(device, m_BufferMemory, nullptr);
	}

}