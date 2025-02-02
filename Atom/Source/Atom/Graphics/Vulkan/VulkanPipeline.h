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

		virtual void Set(uint32_t binding, StorageBuffer* storageBuffer) const override;
		virtual void Set(uint32_t binding, Texture* texture, uint32_t slot) const override;
	private:
		void CreatePipelineLayout(VkDevice device);
		void CreateGraphicsPipeline(VkDevice device);
	private:
		std::vector<VkDescriptorSet> m_DescriptorSets;

		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_GraphicsPipeline = VK_NULL_HANDLE;

		friend class VulkanRenderCommand;
	};

}
