#pragma once
#include "Atom/Graphics/RenderCommand.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	namespace Internal
	{

		class VulkanDevice;

	}

	class VulkanRenderCommand : public RenderCommand
	{
	public:
		VulkanRenderCommand();
		virtual ~VulkanRenderCommand() = default;

		virtual void ResetCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const override;
		virtual void BeginCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const override;
		virtual void EndCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const override;
		virtual void SubmitCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex, bool wait = false) const override;

		virtual void RenderStaticPipeline(CommandBuffer* commandBuffer, Pipeline* pipeline, uint32_t vertexCount, uint32_t frameIndex) const override;
	private:
		Internal::VulkanDevice* m_VulkanDevice;
	};

}
