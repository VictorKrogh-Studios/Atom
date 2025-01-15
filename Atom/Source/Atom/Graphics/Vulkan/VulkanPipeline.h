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
		void CreateDescriptorSetLayout(VkDevice device);
		void CreateDescriptorPool(VkDevice device);
		void CreateDescriptorSet(VkDevice device);
		void CreatePipelineLayout(VkDevice device);
		void CreateGraphicsPipeline(VkDevice device);
	private:
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;

		friend class VulkanRenderCommand;
	};

}
