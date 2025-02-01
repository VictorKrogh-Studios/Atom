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

		virtual void BeginRenderPass(CommandBuffer* commandBuffer, RenderPass* renderPass, uint32_t frameIndex) const override;
		virtual void EndRenderPass(CommandBuffer* commandBuffer, uint32_t frameIndex) const override;
		virtual void SetViewport(CommandBuffer* commandBuffer, RenderPass* renderPass, uint32_t frameIndex) const override;

		virtual void RenderStaticPipeline(CommandBuffer* commandBuffer, Pipeline* pipeline, uint32_t vertexCount, uint32_t frameIndex) const override;
		virtual void DrawVertices(CommandBuffer* commandBuffer, Pipeline* pipeline, VertexBuffer* vertexBuffer, uint32_t vertexCount, uint32_t frameIndex) const override;
		virtual void DrawIndexed(CommandBuffer* commandBuffer, Pipeline* pipeline, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint32_t indexCount, uint32_t frameIndex) const override;
	private:
		Internal::VulkanDevice* m_VulkanDevice;
		VulkanSwapChain* m_SwapChain;

		friend class VulkanImGuiLayer;
	};

}
