#include "ATPCH.h"
#include "VulkanRenderTexture.h"
#include "VulkanGraphicsContext.h"

#include "Internal/VulkanUtils.h"

#include <backends/imgui_impl_vulkan.h>

namespace Atom
{

	VulkanRenderTexture::VulkanRenderTexture(VkImageView vkImageView)
		: m_ImageView(vkImageView)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		CreateSampler(device);

		m_DescriptorSet = ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	VulkanRenderTexture::~VulkanRenderTexture()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		ImGui_ImplVulkan_RemoveTexture(m_DescriptorSet);

		vkDestroySampler(device, m_Sampler, nullptr);
	}

	void* VulkanRenderTexture::GetRaw() const
	{
		return m_DescriptorSet;
	}

	void VulkanRenderTexture::CreateSampler(VkDevice device)
	{
		VkSamplerCreateInfo sampler_info{};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = VK_FILTER_LINEAR;
		sampler_info.minFilter = VK_FILTER_LINEAR;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // outside image bounds just use border color
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.minLod = -1000;
		sampler_info.maxLod = 1000;
		sampler_info.maxAnisotropy = 1.0f;
		VkResult result = vkCreateSampler(device, &sampler_info, nullptr, &m_Sampler);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

}
