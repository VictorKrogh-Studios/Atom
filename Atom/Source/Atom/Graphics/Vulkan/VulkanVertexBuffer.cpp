#include "ATPCH.h"
#include "VulkanVertexBuffer.h"
#include "VulkanGraphicsContext.h"

namespace Atom
{

	VulkanVertexBuffer::VulkanVertexBuffer(uint64_t size, void* vertices)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;
		Internal::VulkanPhysicalDevice* physicalDevice = VulkanGraphicsContext::GetPhysicalDevice();

		CreateVertexBuffer(device, size, vertices);

		AllocateBufferMemory(device, physicalDevice);

		void* data;
		vkMapMemory(device, m_BufferMemory, 0, size, 0, &data);
		memcpy(data, vertices, size);
		vkUnmapMemory(device, m_BufferMemory);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		vkDestroyBuffer(device, m_Buffer, nullptr);
		vkFreeMemory(device, m_BufferMemory, nullptr);
	}

	void VulkanVertexBuffer::CreateVertexBuffer(VkDevice device, uint64_t size, void* vertices)
	{
		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult result = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &m_Buffer);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanVertexBuffer::AllocateBufferMemory(VkDevice device, Internal::VulkanPhysicalDevice* physicalDevice)
	{
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, m_Buffer, &memRequirements);

		VkMemoryAllocateInfo memoryAllocateInfo{};
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = memRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = physicalDevice->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VkResult result = vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &m_BufferMemory);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		result = vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

}