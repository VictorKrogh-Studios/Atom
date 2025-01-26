#include "ATPCH.h"
#include "VulkanStorageBuffer.h"
#include "VulkanGraphicsContext.h"

namespace Atom
{

	VulkanStorageBuffer::VulkanStorageBuffer(uint64_t size)
		: StorageBuffer(size)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		m_Buffers.resize(3);
		m_BufferMemories.resize(3);
		for (size_t i = 0; i < 3; i++)
		{
			CreateBuffer(device, size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffers[i], m_BufferMemories[i]);
		}
	}

	VulkanStorageBuffer::~VulkanStorageBuffer()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		for (size_t i = 0; i < 3; i++)
		{
			vkDestroyBuffer(device, m_Buffers[i], nullptr);
			vkFreeMemory(device, m_BufferMemories[i], nullptr);
		}
		m_Buffers.clear();
		m_BufferMemories.clear();
	}

	void VulkanStorageBuffer::Upload(uint64_t size, void* data, uint32_t index) const
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		void* mappedData;
		vkMapMemory(device, m_BufferMemories[index], 0, size, 0, &mappedData);
		memcpy(mappedData, data, size);
		vkUnmapMemory(device, m_BufferMemories[index]);
	}

}