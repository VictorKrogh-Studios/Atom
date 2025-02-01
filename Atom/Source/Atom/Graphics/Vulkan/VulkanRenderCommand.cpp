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
	}

	void VulkanRenderCommand::BeginRenderPass(CommandBuffer* commandBuffer, RenderPass* renderPass, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
		VulkanRenderPass* vulkanRenderPass = static_cast<VulkanRenderPass*>(renderPass);
		VulkanSwapChain* vulkanSwapChain = VulkanSwapChain::Get();

		const VkClearValue clearValue = vulkanRenderPass->GetVkClearValue();
		const VkExtent2D renderAreaExtent = vulkanRenderPass->GetRenderAreaExtent();
		const VkFramebuffer frameBuffer = vulkanRenderPass->GetVkFramebuffer();

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
			SetViewport(commandBuffer, renderPass, frameIndex);
		}

	}

	void VulkanRenderCommand::EndRenderPass(CommandBuffer* commandBuffer, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);

		vkCmdEndRenderPass(vulkanCommandBuffer->m_CommandBuffers[frameIndex]);
	}

	void VulkanRenderCommand::SetViewport(CommandBuffer* commandBuffer, RenderPass* renderPass, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
		VulkanRenderPass* vulkanRenderPass = static_cast<VulkanRenderPass*>(renderPass);

		const VkExtent2D renderAreaExtent = vulkanRenderPass->GetRenderAreaExtent();

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)renderAreaExtent.width;
		viewport.height = (float)renderAreaExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vkCmdSetViewport(vulkanCommandBuffer->m_CommandBuffers[frameIndex], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = renderAreaExtent;

		vkCmdSetScissor(vulkanCommandBuffer->m_CommandBuffers[frameIndex], 0, 1, &scissor);
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

}