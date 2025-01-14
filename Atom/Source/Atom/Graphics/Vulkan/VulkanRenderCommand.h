#pragma once
#include "Atom/Graphics/RenderCommand.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanRenderCommand : public RenderCommand
	{
	public:
		VulkanRenderCommand();
		virtual ~VulkanRenderCommand();

		virtual void ResetCommandBuffer(CommandBuffer* commandBuffer) override;
		virtual void BeginCommandBuffer(CommandBuffer* commandBuffer) override;
		virtual void EndCommandBuffer(CommandBuffer* commandBuffer) override;

		virtual void RenderStaticPipeline(CommandBuffer* commandBuffer, Pipeline* pipeline, uint32_t vertexCount) override;
	};

}
