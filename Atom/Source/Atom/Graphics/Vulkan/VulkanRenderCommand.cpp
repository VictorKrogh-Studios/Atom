#include "ATPCH.h"
#include "VulkanRenderCommand.h"
#include "VulkanGraphicsContext.h"

#include "Atom/Graphics/Vulkan/VulkanCommandBuffer.h"
#include "Atom/Graphics/Vulkan/VulkanPipeline.h"

namespace Atom
{

	VulkanRenderCommand::VulkanRenderCommand()
	{
	}

	VulkanRenderCommand::~VulkanRenderCommand()
	{
	}

	void VulkanRenderCommand::ResetCommandBuffer(CommandBuffer* commandBuffer)
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

		VkResult result = vkResetCommandBuffer(vulkanCommandBuffer->m_CommandBuffer, 0);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanRenderCommand::BeginCommandBuffer(CommandBuffer* commandBuffer)
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; 
		// commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkResult result = vkBeginCommandBuffer(vulkanCommandBuffer->m_CommandBuffer, &commandBufferBeginInfo);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanRenderCommand::EndCommandBuffer(CommandBuffer* commandBuffer)
	{
		Internal::VulkanDevice* vulkanDevice = VulkanGraphicsContext::GetDevice();
		VulkanSwapChain* vulkanSwapChain = VulkanSwapChain::Get();

		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

		VkResult result = vkEndCommandBuffer(vulkanCommandBuffer->m_CommandBuffer);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

		VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = &waitStageMask;

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &vulkanSwapChain->m_ImageAvailableSemaphores[vulkanSwapChain->m_CurrentFrameIndex];

		submitInfo.pCommandBuffers = &vulkanCommandBuffer->m_CommandBuffer;
		submitInfo.commandBufferCount = 1;

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &vulkanSwapChain->m_RenderFinishedSemaphores[vulkanSwapChain->m_CurrentFrameIndex];

		result = vkQueueSubmit(vulkanDevice->m_GraphicsQueue, 1, &submitInfo, vulkanSwapChain->m_Fences[vulkanSwapChain->m_CurrentFrameIndex]);

		vkDeviceWaitIdle(vulkanDevice->m_Device);
	}

	void VulkanRenderCommand::RenderStaticPipeline(CommandBuffer* commandBuffer, Pipeline* pipeline, uint32_t vertexCount)
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
		VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);
		VulkanSwapChain* vulkanSwapChain = VulkanSwapChain::Get();

		constexpr VkClearColorValue clearColor = { 0.2f, 0.5f, 0.8f, 1.0f };
		constexpr VkClearValue clearValue = { clearColor };

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValue;
		renderPassBeginInfo.renderArea = {
			{ 0, 0 },
			{ vulkanSwapChain->m_Width, vulkanSwapChain->m_Height }
		};
		renderPassBeginInfo.framebuffer = vulkanSwapChain->m_Framebuffers[vulkanSwapChain->m_CurrentImageIndex];
		renderPassBeginInfo.renderPass = vulkanSwapChain->m_RenderPass; // vulkanPipeline->m_RenderPass;

		vkCmdBeginRenderPass(vulkanCommandBuffer->m_CommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)vulkanSwapChain->m_Width;
		viewport.height = (float)vulkanSwapChain->m_Height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = { vulkanSwapChain->m_Width, vulkanSwapChain->m_Height };

		vkCmdSetViewport(vulkanCommandBuffer->m_CommandBuffer, 0, 1, &viewport);

		vkCmdSetScissor(vulkanCommandBuffer->m_CommandBuffer, 0, 1, &scissor);

		vkCmdBindPipeline(vulkanCommandBuffer->m_CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->m_GraphicsPipeline);

		vkCmdDraw(vulkanCommandBuffer->m_CommandBuffer, vertexCount, 1, 0, 0);

		vkCmdEndRenderPass(vulkanCommandBuffer->m_CommandBuffer);
	}

}