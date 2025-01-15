#include "ATPCH.h"
#include "VulkanRenderCommand.h"
#include "VulkanGraphicsContext.h"

#include "Atom/Graphics/Vulkan/VulkanCommandBuffer.h"
#include "Atom/Graphics/Vulkan/VulkanPipeline.h"

namespace Atom
{

	VulkanRenderCommand::VulkanRenderCommand()
	{
		m_VulkanDevice = VulkanGraphicsContext::GetDevice();
	}

	void VulkanRenderCommand::ResetCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

		VkResult result = vkResetCommandBuffer(vulkanCommandBuffer->m_CommandBuffers[frameIndex], 0);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanRenderCommand::BeginCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkResult result = vkBeginCommandBuffer(vulkanCommandBuffer->m_CommandBuffers[frameIndex], &commandBufferBeginInfo);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanRenderCommand::EndCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

		VkResult result = vkEndCommandBuffer(vulkanCommandBuffer->m_CommandBuffers[frameIndex]);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanRenderCommand::SubmitCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex, bool wait) const
	{
		VulkanSwapChain* vulkanSwapChain = VulkanSwapChain::Get();

		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

		const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

		VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = &waitStageMask;

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &vulkanSwapChain->m_ImageAvailableSemaphores[frameIndex];

		submitInfo.pCommandBuffers = &vulkanCommandBuffer->m_CommandBuffers[frameIndex];
		submitInfo.commandBufferCount = 1;

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &vulkanSwapChain->m_RenderFinishedSemaphores[frameIndex];

		VkResult result = vkQueueSubmit(m_VulkanDevice->m_GraphicsQueue, 1, &submitInfo, vulkanSwapChain->m_Fences[frameIndex]);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		//if (wait)
		//{
		//	vkDeviceWaitIdle(m_VulkanDevice->m_Device);
		//}
	}

	void VulkanRenderCommand::RenderStaticPipeline(CommandBuffer* commandBuffer, Pipeline* pipeline, uint32_t vertexCount, uint32_t frameIndex) const
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

		vkCmdBeginRenderPass(vulkanCommandBuffer->m_CommandBuffers[frameIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

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

		vkCmdSetViewport(vulkanCommandBuffer->m_CommandBuffers[frameIndex], 0, 1, &viewport);

		vkCmdSetScissor(vulkanCommandBuffer->m_CommandBuffers[frameIndex], 0, 1, &scissor);

		vkCmdBindPipeline(vulkanCommandBuffer->m_CommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->m_GraphicsPipeline);

		vkCmdDraw(vulkanCommandBuffer->m_CommandBuffers[frameIndex], vertexCount, 1, 0, 0);

		vkCmdEndRenderPass(vulkanCommandBuffer->m_CommandBuffers[frameIndex]);
	}

}