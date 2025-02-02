#pragma once
#include "Atom/Graphics/Texture.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanRenderTexture : public RenderTexture
	{
	public:
		VulkanRenderTexture(VkImageView vkImageView);
		virtual ~VulkanRenderTexture();

		virtual void* GetRaw() const override;
	private:
		void CreateSampler(VkDevice device);
	private:
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;
		VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
	};

}
