#include "ATPCH.h"
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicsContext.h"
#include "Internal/VulkanPhysicalDevice.h"

namespace Atom
{

	VulkanCommandBuffer::VulkanCommandBuffer()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;
		Internal::VulkanPhysicalDevice* vulkanPhysicalDevice = VulkanGraphicsContext::GetPhysicalDevice();

		CreateCommandPool(device, vulkanPhysicalDevice);

		CreateCommandBuffer(device);
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		vkFreeCommandBuffers(device, m_CommandPool, 1, &m_CommandBuffer);
		vkDestroyCommandPool(device, m_CommandPool, nullptr);
	}

	void VulkanCommandBuffer::CreateCommandPool(VkDevice device, Internal::VulkanPhysicalDevice* physicalDevice)
	{
		// TODO: Maybe move Command Pool into VulkanGraphicsContext?

		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = physicalDevice->m_QueueFamilyIndices.Graphics;

		VkResult result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &m_CommandPool);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanCommandBuffer::CreateCommandBuffer(VkDevice device)
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandBufferCount = 1;
		commandBufferAllocateInfo.commandPool = m_CommandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &m_CommandBuffer);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

}