#pragma once
#include "Atom/Graphics/Pipeline.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanPipeline : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineOptions& options);
		virtual ~VulkanPipeline();
	private:
		void CreatePipelineLayout(VkDevice device);
		void CreateRenderPass(VkDevice device);
		void CreateGraphicsPipeline(VkDevice device);
	private:
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;
	};

}
