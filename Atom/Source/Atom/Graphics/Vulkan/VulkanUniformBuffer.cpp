#include "ATPCH.h"
#include "VulkanUniformBuffer.h"
#include "VulkanGraphicsContext.h"

namespace Atom
{

	VulkanUniformBuffer::VulkanUniformBuffer(uint64_t size, uint32_t framesInFlight)
		: UniformBuffer(size)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		m_Buffers.resize(framesInFlight);
		m_BufferMemories.resize(framesInFlight);
		m_BufferMemoriesMapped.resize(framesInFlight);

		for (uint32_t i = 0; i < framesInFlight; i++)
		{
			CreateBuffer(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_Buffers[i], m_BufferMemories[i]);

			vkMapMemory(device, m_BufferMemories[i], 0, size, 0, &m_BufferMemoriesMapped[i]);
		}
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		for (uint32_t i = 0; i < m_BufferMemoriesMapped.size(); i++)
		{
			vkUnmapMemory(device, m_BufferMemories[i]);
		}

		for (size_t i = 0; i < m_Buffers.size(); i++)
		{
			vkDestroyBuffer(device, m_Buffers[i], nullptr);
			vkFreeMemory(device, m_BufferMemories[i], nullptr);
		}
	}

	void VulkanUniformBuffer::Upload(void* data, uint64_t size, uint32_t frameIndex) const
	{
		memcpy(m_BufferMemoriesMapped[frameIndex], data, size);
	}

}