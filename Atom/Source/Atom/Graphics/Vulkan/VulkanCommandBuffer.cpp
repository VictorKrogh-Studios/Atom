#include "ATPCH.h"
#include "VulkanCommandBuffer.h"
#include "VulkanGraphicsContext.h"
#include "Internal/VulkanPhysicalDevice.h"

#include "Atom/Graphics/Vulkan/VulkanUtils.h"

namespace Atom
{

	VulkanCommandBuffer::VulkanCommandBuffer(const CommandBufferCreateInfo& commandBufferCreateInfo, uint32_t count)
		: CommandBuffer(commandBufferCreateInfo, count)
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

	void VulkanCommandBuffer::Reset(uint32_t index) const
	{
		VkResult result = vkResetCommandBuffer(m_CommandBuffers[index], 0);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanCommandBuffer::Begin(uint32_t index) const
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = Vulkan::Utils::GetVkCommandBufferUsageFlags(m_CommandBufferCreateInfo.Usage);

		VkResult result = vkBeginCommandBuffer(m_CommandBuffers[index], &commandBufferBeginInfo);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanCommandBuffer::End(uint32_t index) const
	{
		VkResult result = vkEndCommandBuffer(m_CommandBuffers[index]);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanCommandBuffer::Submit(uint32_t index) const
	{
		AT_CORE_ASSERT(m_CommandBufferCreateInfo.Level == Enumerations::CommandBufferLevel::Primary, "Command buffer must be PRIMARY");

		Internal::VulkanDevice* vulkanDevice = VulkanGraphicsContext::GetDevice();

		// const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pCommandBuffers = &m_CommandBuffers[index];
		submitInfo.commandBufferCount = 1;

		VkFence fence = VK_NULL_HANDLE;
		if (m_CommandBufferCreateInfo.TargetSwapChain)
		{
			VulkanSwapChain* vulkanSwapChain = VulkanSwapChain::Get();

			VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			submitInfo.pWaitDstStageMask = &waitStageMask;

			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = &vulkanSwapChain->m_SwapChainSemaphores[index].ImageAvailableSemaphore;

			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = &vulkanSwapChain->m_SwapChainSemaphores[index].RenderFinishedSemaphore;

			fence = vulkanSwapChain->m_Fences[index];

			//vkWaitForFences(vulkanDevice->GetVkDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
			//vkResetFences(vulkanDevice->GetVkDevice(), 1, &fence);
		}

		VkResult result = vkQueueSubmit(vulkanDevice->m_GraphicsQueue, 1, &submitInfo, fence);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanCommandBuffer::CreateCommandBuffer(VkDevice device)
	{
		m_CommandBuffers.resize(m_Count);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandBufferCount = m_Count;
		commandBufferAllocateInfo.commandPool = VulkanGraphicsContext::Get()->m_GraphicsCommandPool;
		commandBufferAllocateInfo.level = Vulkan::Utils::GetVkCommandBufferLevel(m_CommandBufferCreateInfo.Level);

		VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, m_CommandBuffers.data());
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

}