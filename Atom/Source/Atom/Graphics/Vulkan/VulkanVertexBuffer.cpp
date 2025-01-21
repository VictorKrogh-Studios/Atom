#include "ATPCH.h"
#include "VulkanVertexBuffer.h"
#include "VulkanGraphicsContext.h"

#include "Atom/Graphics/Vulkan/VulkanUtils.h"

namespace Atom
{

	VulkanVertexBuffer::VulkanVertexBuffer(const VertexBufferCreateInfo& createInfo)
		: m_CreateInfo(createInfo)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		VkBufferUsageFlags bufferUsageFlag = Vulkan::Utils::GetVkBufferUsageFlags(m_CreateInfo.Usage);
		CreateBuffer(device, m_CreateInfo.Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | bufferUsageFlag, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Buffer, m_BufferMemory);

		CreateBuffer(device, m_CreateInfo.Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_UploadBuffer, m_UploadBufferMemory);

		if (m_CreateInfo.Vertices != nullptr)
		{
			Upload(m_CreateInfo.Size, m_CreateInfo.Vertices);
		}
	}

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

		vkDestroyBuffer(device, m_UploadBuffer, nullptr);
		vkFreeMemory(device, m_UploadBufferMemory, nullptr);

		vkDestroyBuffer(device, m_Buffer, nullptr);
		vkFreeMemory(device, m_BufferMemory, nullptr);
	}

	void VulkanVertexBuffer::Upload(uint32_t size, void* vertices) const
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		void* data;
		vkMapMemory(device, m_UploadBufferMemory, 0, size, 0, &data);
		memcpy(data, vertices, size);
		vkUnmapMemory(device, m_UploadBufferMemory);

		CopyBuffer(device, m_UploadBuffer, m_Buffer, size);
	}

}