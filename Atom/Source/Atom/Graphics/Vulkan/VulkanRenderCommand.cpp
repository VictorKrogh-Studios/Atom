#include "ATPCH.h"
#include "VulkanRenderCommand.h"
#include "VulkanGraphicsContext.h"

namespace Atom
{

	VulkanRenderCommand::VulkanRenderCommand()
	{
		VulkanPhysicalDevice* physicalDevice = VulkanGraphicsContext::GetPhysicalDevice();
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		CreateCommandPool(device, physicalDevice);

		CreateCommandBuffer(device);
	}

	VulkanRenderCommand::~VulkanRenderCommand()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		vkFreeCommandBuffers(device, m_CommandPool, 1, &m_CommandBuffer);
		vkDestroyCommandPool(device, m_CommandPool, nullptr);
	}

	void VulkanRenderCommand::BeginRecording()
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkResult result = vkResetCommandBuffer(m_CommandBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		result = vkBeginCommandBuffer(m_CommandBuffer, &commandBufferBeginInfo);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanRenderCommand::EndRecording()
	{
		VkResult result = vkEndCommandBuffer(m_CommandBuffer);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		VulkanDevice* vulkanDevice = VulkanGraphicsContext::GetDevice();

		const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

		VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = &waitStageMask;
		submitInfo.pWaitSemaphores = &m_ImageAvailableSemaphores[m_CurrentFrameIndex];
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrameIndex];
		submitInfo.signalSemaphoreCount = 1;

		// TODO: We need to find the Command buffers to submit this frame!
		//submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrameIndex].CommandBuffer;
		//submitInfo.commandBufferCount = 1;

		VkResult result = vkResetFences(vulkanDevice->m_Device, 1, &m_Fences[m_CurrentFrameIndex]);

		result = vkQueueSubmit(vulkanDevice->m_GraphicsQueue, 1, &submitInfo, m_Fences[m_CurrentFrameIndex]);
	}

	void VulkanRenderCommand::RenderStaticPipeline(Pipeline* pipeline, uint32_t vertexCount)
	{
	}

	void VulkanRenderCommand::CreateCommandPool(VkDevice device, VulkanPhysicalDevice* physicalDevice)
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = physicalDevice->m_QueueFamilyIndices.Graphics;

		VkResult result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &m_CommandPool);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanRenderCommand::CreateCommandBuffer(VkDevice device)
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