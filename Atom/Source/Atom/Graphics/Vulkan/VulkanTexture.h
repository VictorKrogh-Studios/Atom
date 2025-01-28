#pragma once
#include "Atom/Graphics/Texture.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::filesystem::path& filepath);
		virtual ~VulkanTexture();
	private:
		void CreateImageView(VkDevice device);
		void CreateSampler(VkDevice device);
	private:
		VkImage m_Image = VK_NULL_HANDLE;
		VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;

		friend class VulkanPipeline;
		friend class VulkanShader;
	};

}
