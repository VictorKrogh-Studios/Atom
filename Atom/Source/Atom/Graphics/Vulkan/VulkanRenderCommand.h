#pragma once
#include "Atom/Graphics/RenderCommand.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	namespace Internal
	{

		class VulkanDevice;

	}

	class VulkanSwapChain;

	class VulkanRenderCommand : public RenderCommand
	{
	public:
		VulkanRenderCommand();
		virtual ~VulkanRenderCommand() = default;

		virtual void ResetCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const override;
		virtual void BeginCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const override;
		virtual void EndCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const override;
		virtual void SubmitCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex, bool wait = false) const override;

		virtual void BeginRenderPass(CommandBuffer* commandBuffer, RenderPass* renderPass, uint32_t frameIndex) const override;
		virtual void EndRenderPass(CommandBuffer* commandBuffer, uint32_t frameIndex) const override;

		virtual void RenderStaticPipeline(CommandBuffer* commandBuffer, Pipeline* pipeline, uint32_t vertexCount, uint32_t frameIndex) const override;
		virtual void DrawVertices(CommandBuffer* commandBuffer, Pipeline* pipeline, VertexBuffer* vertexBuffer, uint32_t vertexCount, uint32_t frameIndex) const override;
		virtual void DrawIndexed(CommandBuffer* commandBuffer, Pipeline* pipeline, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint32_t indexCount, uint32_t frameIndex) const override;

	private: // Should only be called from Renderer!
		virtual void BeginFrame(uint32_t frameIndex) const override;
		virtual void EndFrame(uint32_t frameIndex) const override;

	private:
		void CreateDrawCommandBuffers(VkDevice device, uint32_t count);
	private:
		Internal::VulkanDevice* m_VulkanDevice;
		VulkanSwapChain* m_SwapChain;

		std::vector<VkCommandBuffer> m_DrawCommandBuffers;

		friend class VulkanImGuiLayer;
	};

}
