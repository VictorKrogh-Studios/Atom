#include "ATPCH.h"
#include "VulkanTexture.h"
#include "VulkanGraphicsContext.h"
#include "Internal/VulkanPhysicalDevice.h"

#include "Internal/VulkanUtils.h"

#include <backends/imgui_impl_vulkan.h>

#include <stb_image.h>

namespace Atom
{

	VulkanTexture::VulkanTexture(const std::filesystem::path& filepath, const TextureCreateInfo& createInfo)
		: Texture(0, 0, 0, createInfo)
	{
		int32_t width, height, channels;
		unsigned char* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (!data)
		{
			AT_CORE_ERROR("Failed to load image '{0}'", filepath.string());
			return;
		}

		m_Width = width;
		m_Height = height;
		m_Channels = channels;

		uint64_t size = m_Width * m_Height * 4; //m_Channels;

		VkDevice device = VulkanGraphicsContext::Get()->GetDevice()->GetVkDevice();

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
		Internal::VulkanUtils::CreateDefaultStagingBuffer(size, stagingBuffer, stagingBufferMemory);

		void* mappedData;
		vkMapMemory(device, stagingBufferMemory, 0, size, 0, &mappedData);
		memcpy(mappedData, data, size);
		vkUnmapMemory(device, stagingBufferMemory);

		stbi_image_free(data);

		Internal::VulkanUtils::CreateImage2D(m_Width, m_Height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);

		Internal::VulkanUtils::TransitionImageLayout(VulkanGraphicsContext::Get()->m_GraphicsCommandPool, VulkanGraphicsContext::GetDevice()->m_GraphicsQueue, m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		Internal::VulkanUtils::CopyBufferToImage(VulkanGraphicsContext::Get()->m_GraphicsCommandPool, VulkanGraphicsContext::GetDevice()->m_GraphicsQueue, stagingBuffer, m_Image, m_Width, m_Height);

		Internal::VulkanUtils::TransitionImageLayout(VulkanGraphicsContext::Get()->m_GraphicsCommandPool, VulkanGraphicsContext::GetDevice()->m_GraphicsQueue, m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		CreateImageView(device);
		CreateSampler(device);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, void* data, const TextureCreateInfo& createInfo)
		: Texture(width, height, 4, createInfo)
	{
		uint64_t size = m_Width * m_Height * 4; //m_Channels;

		VkDevice device = VulkanGraphicsContext::Get()->GetDevice()->GetVkDevice();

		VkBuffer stagingBuffer = VK_NULL_HANDLE;
		VkDeviceMemory stagingBufferMemory = VK_NULL_HANDLE;
		Internal::VulkanUtils::CreateDefaultStagingBuffer(size, stagingBuffer, stagingBufferMemory);

		void* mappedData;
		vkMapMemory(device, stagingBufferMemory, 0, size, 0, &mappedData);
		memcpy(mappedData, data, size);
		vkUnmapMemory(device, stagingBufferMemory);

		Internal::VulkanUtils::CreateImage2D(m_Width, m_Height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_ImageMemory);

		Internal::VulkanUtils::TransitionImageLayout(VulkanGraphicsContext::Get()->m_GraphicsCommandPool, VulkanGraphicsContext::GetDevice()->m_GraphicsQueue, m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		Internal::VulkanUtils::CopyBufferToImage(VulkanGraphicsContext::Get()->m_GraphicsCommandPool, VulkanGraphicsContext::GetDevice()->m_GraphicsQueue, stagingBuffer, m_Image, m_Width, m_Height);

		Internal::VulkanUtils::TransitionImageLayout(VulkanGraphicsContext::Get()->m_GraphicsCommandPool, VulkanGraphicsContext::GetDevice()->m_GraphicsQueue, m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		CreateImageView(device);
		CreateSampler(device);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	VulkanTexture::VulkanTexture(uint32_t width, uint32_t height, VkImage vkImage, VkImageView vkImageView)
		: Texture(width, height, 0, {})
	{
		VkDevice device = VulkanGraphicsContext::Get()->GetDevice()->GetVkDevice();

		m_Image = vkImage;
		m_ImageView = vkImageView;

		CreateSampler(device);

		m_ImGuiDescriptorSet = ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	VulkanTexture::~VulkanTexture()
	{
		VkDevice device = VulkanGraphicsContext::Get()->GetDevice()->GetVkDevice();

		vkDestroySampler(device, m_Sampler, nullptr);

		if (!m_ImGuiDescriptorSet)
		{
			vkDestroyImageView(device, m_ImageView, nullptr);
			vkDestroyImage(device, m_Image, nullptr);
			vkFreeMemory(device, m_ImageMemory, nullptr);
		}
	}

	TextureNativeHandle VulkanTexture::GetNativeHandle()
	{
		if (!m_ImGuiDescriptorSet)
		{
			m_ImGuiDescriptorSet = ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}

		return m_ImGuiDescriptorSet;
	}

	void VulkanTexture::CreateImageView(VkDevice device)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &m_ImageView);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanTexture::CreateSampler(VkDevice device)
	{
		const VkPhysicalDeviceProperties& props = VulkanGraphicsContext::GetPhysicalDevice()->GetProperties();
		const VkPhysicalDeviceFeatures& features = VulkanGraphicsContext::GetPhysicalDevice()->GetFeatures();

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		samplerInfo.anisotropyEnable = features.samplerAnisotropy;
		samplerInfo.maxAnisotropy = 1.0f;
		if (samplerInfo.anisotropyEnable)
		{
			samplerInfo.maxAnisotropy = props.limits.maxSamplerAnisotropy;
		}

		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		VkResult result = vkCreateSampler(device, &samplerInfo, nullptr, &m_Sampler);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

}
