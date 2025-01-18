#include "ATPCH.h"
#include "VulkanRenderCommand.h"
#include "VulkanGraphicsContext.h"

#include "Atom/Graphics/Vulkan/VulkanSwapChain.h"

#include "Atom/Graphics/Vulkan/VulkanCommandBuffer.h"
#include "Atom/Graphics/Vulkan/VulkanPipeline.h"
#include "Atom/Graphics/Vulkan/VulkanVertexBuffer.h"
#include "Atom/Graphics/Vulkan/VulkanIndexBuffer.h"
#include "Atom/Graphics/Vulkan/VulkanRenderPass.h"

namespace Atom
{

	VulkanRenderCommand::VulkanRenderCommand()
	{
		m_VulkanDevice = VulkanGraphicsContext::GetDevice();
		m_SwapChain = VulkanSwapChain::Get();

		CreateDrawCommandBuffers(m_VulkanDevice->m_Device, 3);
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
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		//commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		VkResult result = vkBeginCommandBuffer(vulkanCommandBuffer->m_CommandBuffers[frameIndex], &commandBufferBeginInfo);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanRenderCommand::EndCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

		VkResult result = vkEndCommandBuffer(vulkanCommandBuffer->m_CommandBuffers[frameIndex]);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	static VkFence s_WaitFence = VK_NULL_HANDLE;

	void VulkanRenderCommand::SubmitCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex, bool wait) const
	{
		//if (s_WaitFence == VK_NULL_HANDLE)
		//{
		//	VkFenceCreateInfo fenceCreateInfo{};
		//	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		//	vkCreateFence(VulkanGraphicsContext::GetDevice()->GetVkDevice(), &fenceCreateInfo, nullptr, &s_WaitFence);
		//}

		VulkanSwapChain* vulkanSwapChain = VulkanSwapChain::Get();

		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

		const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

		// VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		//{
		//	VkSubmitInfo submitInfo = {};
		//	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		//	submitInfo.pCommandBuffers = &vulkanCommandBuffer->m_CommandBuffers[frameIndex];
		//	submitInfo.commandBufferCount = 1;

		//	vkWaitForFences(m_VulkanDevice->GetVkDevice(), 1, &s_WaitFence, VK_TRUE, UINT64_MAX);
		//	vkResetFences(m_VulkanDevice->GetVkDevice(), 1, &s_WaitFence);

		//	VkResult result = vkQueueSubmit(m_VulkanDevice->m_GraphicsQueue, 1, &submitInfo, s_WaitFence);
		//	AT_CORE_ASSERT(result == VK_SUCCESS);
		//}

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		//submitInfo.pWaitDstStageMask = &waitStageMask;

		//submitInfo.waitSemaphoreCount = 1;
		//submitInfo.pWaitSemaphores = &vulkanSwapChain->m_ImageAvailableSemaphores[frameIndex];

		submitInfo.pCommandBuffers = &vulkanCommandBuffer->m_CommandBuffers[frameIndex];
		submitInfo.commandBufferCount = 1;

		//submitInfo.signalSemaphoreCount = 1;
		//submitInfo.pSignalSemaphores = &vulkanSwapChain->m_RenderFinishedSemaphores[frameIndex];

		//vkWaitForFences(m_VulkanDevice->GetVkDevice(), 1, &s_WaitFence, VK_TRUE, UINT64_MAX);
		//vkResetFences(m_VulkanDevice->GetVkDevice(), 1, &s_WaitFence);

		VkResult result = vkQueueSubmit(m_VulkanDevice->m_GraphicsQueue, 1, &submitInfo, s_WaitFence);
		//VkResult result = vkQueueSubmit(m_VulkanDevice->m_GraphicsQueue, 1, &submitInfo, vulkanSwapChain->m_Fences[frameIndex]);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		//if (wait)
		//{
		//	vkDeviceWaitIdle(m_VulkanDevice->m_Device);
		//}
	}

	void VulkanRenderCommand::BeginRenderPass(CommandBuffer* commandBuffer, RenderPass* renderPass, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
		VulkanRenderPass* vulkanRenderPass = static_cast<VulkanRenderPass*>(renderPass);
		VulkanSwapChain* vulkanSwapChain = VulkanSwapChain::Get();

		const VkClearValue clearValue = vulkanRenderPass->GetVkClearValue();
		const VkExtent2D renderAreaExtent = vulkanRenderPass->GetRenderAreaExtent();
		VkFramebuffer frameBuffer = VK_NULL_HANDLE;
		if (vulkanRenderPass->m_CreateInfo.TargetSwapChain)
		{
			frameBuffer = vulkanSwapChain->GetCurrentFramebuffer();
		}

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValue;
		renderPassBeginInfo.renderArea = {
			{ 0, 0 },
			renderAreaExtent
		};
		renderPassBeginInfo.framebuffer = frameBuffer;
		renderPassBeginInfo.renderPass = vulkanRenderPass->GetVkRenderPass();

		vkCmdBeginRenderPass(vulkanCommandBuffer->m_CommandBuffers[frameIndex], &renderPassBeginInfo, vulkanRenderPass->GetVkSubpassContents());

		if (vulkanRenderPass->m_CreateInfo.ImplicitSetViewport)
		{
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)renderAreaExtent.width;
			viewport.height = (float)renderAreaExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vkCmdSetViewport(vulkanCommandBuffer->m_CommandBuffers[frameIndex], 0, 1, &viewport);
		}

		if (vulkanRenderPass->m_CreateInfo.ImplicitSetScissor)
		{
			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = renderAreaExtent;

			vkCmdSetScissor(vulkanCommandBuffer->m_CommandBuffers[frameIndex], 0, 1, &scissor);
		}
	}

	void VulkanRenderCommand::EndRenderPass(CommandBuffer* commandBuffer, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

		vkCmdEndRenderPass(vulkanCommandBuffer->m_CommandBuffers[frameIndex]);
	}

	void VulkanRenderCommand::RenderStaticPipeline(CommandBuffer* commandBuffer, Pipeline* pipeline, uint32_t vertexCount, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
		VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);

		vkCmdBindPipeline(vulkanCommandBuffer->m_CommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->m_GraphicsPipeline);

		vkCmdDraw(vulkanCommandBuffer->m_CommandBuffers[frameIndex], vertexCount, 1, 0, 0);
	}

	void VulkanRenderCommand::DrawVertices(CommandBuffer* commandBuffer, Pipeline* pipeline, VertexBuffer* vertexBuffer, uint32_t vertexCount, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
		VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);
		VulkanVertexBuffer* vulkanVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer);

		vkCmdBindPipeline(vulkanCommandBuffer->m_CommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->m_GraphicsPipeline);

		uint64_t offset = 0;
		vkCmdBindVertexBuffers(vulkanCommandBuffer->m_CommandBuffers[frameIndex], 0, 1, &vulkanVertexBuffer->m_Buffer, &offset);

		vkCmdDraw(vulkanCommandBuffer->m_CommandBuffers[frameIndex], vertexCount, 1, 0, 0);
	}

	void VulkanRenderCommand::DrawIndexed(CommandBuffer* commandBuffer, Pipeline* pipeline, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint32_t indexCount, uint32_t frameIndex) const
	{
		if (indexCount == 0)
		{
			indexCount = indexBuffer->GetIndexCount();
		}

		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
		VulkanPipeline* vulkanPipeline = static_cast<VulkanPipeline*>(pipeline);
		VulkanVertexBuffer* vulkanVertexBuffer = static_cast<VulkanVertexBuffer*>(vertexBuffer);
		VulkanIndexBuffer* vulkanIndexBuffer = static_cast<VulkanIndexBuffer*>(indexBuffer);

		vkCmdBindPipeline(vulkanCommandBuffer->m_CommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->m_GraphicsPipeline);

		vkCmdBindDescriptorSets(vulkanCommandBuffer->m_CommandBuffers[frameIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanPipeline->m_PipelineLayout, 0, 1, &vulkanPipeline->m_DescriptorSets[frameIndex], 0, nullptr);

		uint64_t offset = 0;
		vkCmdBindVertexBuffers(vulkanCommandBuffer->m_CommandBuffers[frameIndex], 0, 1, &vulkanVertexBuffer->m_Buffer, &offset);

		vkCmdBindIndexBuffer(vulkanCommandBuffer->m_CommandBuffers[frameIndex], vulkanIndexBuffer->m_Buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(vulkanCommandBuffer->m_CommandBuffers[frameIndex], indexCount, 1, 0, 0, 0);
	}

	void VulkanRenderCommand::BeginFrame(uint32_t frameIndex) const
	{
		VkResult result = vkResetCommandBuffer(m_DrawCommandBuffers[frameIndex], 0);
		AT_CORE_ASSERT(result == VK_SUCCESS);

#if 1
		VkCommandBufferBeginInfo commandBufferBeginInfo{};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		//commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		result = vkBeginCommandBuffer(m_DrawCommandBuffers[frameIndex], &commandBufferBeginInfo);
		AT_CORE_ASSERT(result == VK_SUCCESS);
#endif
	}

	void VulkanRenderCommand::EndFrame(uint32_t frameIndex) const
	{
#if 1
		VkResult result = vkEndCommandBuffer(m_DrawCommandBuffers[frameIndex]);
		AT_CORE_ASSERT(result == VK_SUCCESS);
#endif

		VulkanSwapChain::SwapChainSemaphores semaphores = m_SwapChain->GetSwapChainSemaphores(frameIndex);

		VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pWaitDstStageMask = &waitStageMask;

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &semaphores.ImageAvailableSemaphore; // vulkanSwapChain->m_ImageAvailableSemaphores[frameIndex];

		submitInfo.pCommandBuffers = &m_DrawCommandBuffers[frameIndex];
		submitInfo.commandBufferCount = 1;

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &semaphores.RenderFinishedSemaphore; // vulkanSwapChain->m_RenderFinishedSemaphores[frameIndex];

		result = vkQueueSubmit(m_VulkanDevice->m_GraphicsQueue, 1, &submitInfo, m_SwapChain->m_Fences[frameIndex]);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanRenderCommand::CreateDrawCommandBuffers(VkDevice device, uint32_t count)
	{
		m_DrawCommandBuffers.resize(count);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandBufferCount = count;
		commandBufferAllocateInfo.commandPool = VulkanGraphicsContext::Get()->m_GraphicsCommandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, m_DrawCommandBuffers.data());
		AT_CORE_ASSERT(result == VK_SUCCESS);

#if 0
		auto fpSetDebugUtilsObjectNameEXT = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetDeviceProcAddr(device, "vkSetDebugUtilsObjectNameEXT");

		VkDebugUtilsObjectNameInfoEXT nameInfo{};
		nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
		nameInfo.objectType = VK_OBJECT_TYPE_COMMAND_BUFFER;
		nameInfo.pObjectName = "Draw Command Buffer";
		nameInfo.objectHandle = (uint64_t)m_DrawCommandBuffers[0];
		fpSetDebugUtilsObjectNameEXT(device, &nameInfo);
#endif
	}

}