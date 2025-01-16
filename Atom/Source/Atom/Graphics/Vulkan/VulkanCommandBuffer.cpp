#include "ATPCH.h"
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicsContext.h"
#include "Internal/VulkanPhysicalDevice.h"

namespace Atom
{

	VulkanCommandBuffer::VulkanCommandBuffer(uint32_t count)
		: CommandBuffer(count)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		CreateCommandBuffer(device);
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		vkFreeCommandBuffers(device, VulkanGraphicsContext::Get()->m_GraphicsCommandPool, m_CommandBuffers.size(), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	void VulkanCommandBuffer::CreateCommandBuffer(VkDevice device)
	{
		m_CommandBuffers.resize(m_Count);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandBufferCount = m_Count;
		commandBufferAllocateInfo.commandPool = VulkanGraphicsContext::Get()->m_GraphicsCommandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, m_CommandBuffers.data());
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

}