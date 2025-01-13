#pragma once
#include "Atom/Graphics/RenderCommand.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanPhysicalDevice;

	class VulkanRenderCommand : public RenderCommand
	{
	public:
		VulkanRenderCommand();
		virtual ~VulkanRenderCommand();

		virtual void BeginRecording() override;
		virtual void EndRecording() override;

		virtual void RenderStaticPipeline(Pipeline* pipeline, uint32_t vertexCount) override;
	private:
		void CreateCommandPool(VkDevice device, VulkanPhysicalDevice* physicalDevice);
		void CreateCommandBuffer(VkDevice device);
	private:
		VkCommandPool m_CommandPool = VK_NULL_HANDLE;
		VkCommandBuffer m_CommandBuffer = VK_NULL_HANDLE;
	};

}
