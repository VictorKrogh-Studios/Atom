#pragma once
#include "Atom/Graphics/Texture.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::filesystem::path& filepath, const TextureCreateInfo& createInfo);
		VulkanTexture(uint32_t width, uint32_t height, void* data, const TextureCreateInfo& createInfo);
		VulkanTexture(uint32_t width, uint32_t height, VkImage vkImage, VkImageView vkImageView);
		virtual ~VulkanTexture();

		virtual TextureNativeHandle GetNativeHandle() override;
	private:
		void CreateImageView(VkDevice device);
		void CreateSampler(VkDevice device);
	private:
		VkImage m_Image = VK_NULL_HANDLE;
		VkDeviceMemory m_ImageMemory = VK_NULL_HANDLE;
		VkImageView m_ImageView = VK_NULL_HANDLE;
		VkSampler m_Sampler = VK_NULL_HANDLE;

		VkDescriptorSet m_ImGuiDescriptorSet = VK_NULL_HANDLE;

		friend class VulkanPipeline;
		friend class VulkanShader;
	};

}
