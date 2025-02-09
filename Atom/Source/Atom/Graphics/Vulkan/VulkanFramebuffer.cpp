#include "ATPCH.h"
#include "VulkanFramebuffer.h"
#include "VulkanGraphicsContext.h"

#include "VulkanRenderPass.h"
#include "VulkanTexture.h"
#include "VulkanUtils.h"

#include "Internal/VulkanUtils.h"

#include "Atom/Core/Application.h"

namespace Atom
{

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferCreateInfo& createInfo)
		: Framebuffer(createInfo)
	{
		Create(m_CreateInfo.Width, m_CreateInfo.Height);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		Destroy();
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		if (m_Width == width && m_Height == height)
		{
			return; // No need to resize
		}

		Application::SubmitReleaseQueue([&, this, width, height]()
		{
			if (m_Width == width && m_Height == height)
			{
				return; // No need to resize
			}

			VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

			this->Destroy();

			this->Create(width, height);
		});
	}

	Texture* VulkanFramebuffer::GetColorAttachment(uint32_t index) const
	{
		return m_RenderTextures[index];
	}

	void VulkanFramebuffer::Create(uint32_t width, uint32_t height)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		m_Width = width;
		m_Height = height;

		CreateImages(device);
		CreateImageViews(device);
		CreateFramebuffer(device);
		CreateRenderTextures();
	}

	void VulkanFramebuffer::Destroy()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		vkDestroyFramebuffer(device, m_Framebuffer, nullptr);

		for (VkImage image : m_AttachmentImages)
		{
			vkDestroyImage(device, image, nullptr);
		}
		m_AttachmentImages.clear();

		vkDestroyImage(device, m_DepthAttachmentImage, nullptr);
		m_DepthAttachmentImage = nullptr;

		for (VkDeviceMemory memory : m_AttachmentImageMemories)
		{
			vkFreeMemory(device, memory, nullptr);
		}
		m_AttachmentImageMemories.clear();

		vkFreeMemory(device, m_DepthAttachmentImageMemory, nullptr);
		m_DepthAttachmentImageMemory = nullptr;

		for (VkImageView imageView : m_AttachmentImageViews)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}
		m_AttachmentImageViews.clear();

		vkDestroyImageView(device, m_DepthAttachmentImageView, nullptr);
		m_DepthAttachmentImageView = nullptr;

		for (uint32_t i = 0; i < m_RenderTextures.size(); i++)
		{
			delete m_RenderTextures[i];
		}
		m_RenderTextures.clear();
	}

	void VulkanFramebuffer::CreateImages(VkDevice device)
	{
		Internal::VulkanPhysicalDevice* physicalDevice = VulkanGraphicsContext::GetPhysicalDevice();

		if (m_CreateInfo.TargetSwapChain)
		{
			m_AttachmentImages = VulkanSwapChain::Get()->GetVkImages();
			return;
		}

		m_AttachmentImages.resize(0);
		m_AttachmentImageMemories.resize(0);
		uint32_t index = 0;

		for (const auto& attachment : m_CreateInfo.RenderPass->GetAttachments())
		{
			VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;
			if (Internal::VulkanUtils::IsDepthFormat(attachment.Format))
			{
				usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			}
			else
			{
				usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			}
			usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

			VkImageCreateInfo imageCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
			imageCreateInfo.usage = usage;
			imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
			imageCreateInfo.extent.width = m_Width;
			imageCreateInfo.extent.height = m_Height;
			imageCreateInfo.extent.depth = 1;
			imageCreateInfo.mipLevels = 1;
			imageCreateInfo.arrayLayers = 1;
			imageCreateInfo.format = Vulkan::Utils::GetVkFormat(attachment.Format);
			imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageCreateInfo.flags = 0; // Optional

			VkImage image = VK_NULL_HANDLE;
			VkResult result = vkCreateImage(device, &imageCreateInfo, nullptr, &image);
			AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer image");

			VkMemoryRequirements memRequirements;
			vkGetImageMemoryRequirements(device, image, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = physicalDevice->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			VkDeviceMemory memory = VK_NULL_HANDLE;
			result = vkAllocateMemory(device, &allocInfo, nullptr, &memory);
			AT_CORE_ASSERT(result == VK_SUCCESS);

			vkBindImageMemory(device, image, memory, 0);

			if (Internal::VulkanUtils::IsDepthFormat(attachment.Format))
			{
				m_DepthAttachmentImage = image;
				m_DepthAttachmentImageMemory = memory;
			}
			else
			{
				m_AttachmentImages.emplace_back(image);
				m_AttachmentImageMemories.emplace_back(memory);
			}

			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 1;

			Internal::VulkanUtils::TransitionImageLayout(VulkanGraphicsContext::Get()->m_GraphicsCommandPool, VulkanGraphicsContext::GetDevice()->m_GraphicsQueue, image,
				0, 0,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				subresourceRange);

			//Internal::VulkanUtils::TransitionImageLayout(VulkanGraphicsContext::Get()->m_GraphicsCommandPool, VulkanGraphicsContext::GetDevice()->m_GraphicsQueue, m_AttachmentImages[index], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

			// Internal::VulkanUtils::TransitionImageLayout(VulkanGraphicsContext::Get()->m_GraphicsCommandPool, VulkanGraphicsContext::GetDevice()->m_GraphicsQueue, m_AttachmentImages[index], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			index++;
		}
	}

	void VulkanFramebuffer::CreateImageViews(VkDevice device)
	{
		m_AttachmentImageViews.resize(0);
		uint32_t index = 0;

		for (const auto& attachment : m_CreateInfo.RenderPass->GetAttachments())
		{
			bool isDepthFormat = Internal::VulkanUtils::IsDepthFormat(attachment.Format);

			VkImageAspectFlags aspectMask = isDepthFormat ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			if (attachment.Format == Enumerations::ImageFormat::DEPTH24STENCIL8)
			{
				aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}

			VkImageViewCreateInfo imageViewCreateInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			imageViewCreateInfo.format = Vulkan::Utils::GetVkFormat(attachment.Format);
			imageViewCreateInfo.image = isDepthFormat ? m_DepthAttachmentImage : m_AttachmentImages[index];
			imageViewCreateInfo.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.flags = 0;

			VkImageView imageView = VK_NULL_HANDLE;
			VkResult result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView);
			AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer image");

			if (isDepthFormat)
			{
				m_DepthAttachmentImageView = imageView;
			}
			else
			{
				m_AttachmentImageViews.emplace_back(imageView);
			}

			index++;
		}
	}

	void VulkanFramebuffer::CreateFramebuffer(VkDevice device)
	{
		VulkanRenderPass* vulkanRenderPass = static_cast<VulkanRenderPass*>(m_CreateInfo.RenderPass);

		std::vector<VkImageView> attachments(m_AttachmentImages.size());
		for (uint32_t i = 0; i < m_AttachmentImages.size(); i++)
		{
			attachments[i] = m_AttachmentImageViews[i];
		}

		if (m_DepthAttachmentImage)
		{
			attachments.emplace_back(m_DepthAttachmentImageView);
		}

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = vulkanRenderPass->GetVkRenderPass();
		framebufferCreateInfo.width = m_Width;
		framebufferCreateInfo.height = m_Height;
		framebufferCreateInfo.layers = 1;
		framebufferCreateInfo.attachmentCount = (uint32_t)attachments.size();
		framebufferCreateInfo.pAttachments = attachments.data();

		VkResult result = vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &m_Framebuffer);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer!");
	}

	void VulkanFramebuffer::CreateRenderTextures()
	{
		m_RenderTextures.resize(m_AttachmentImageViews.size());
		for (uint32_t i = 0; i < m_RenderTextures.size(); i++)
		{
			delete m_RenderTextures[i];

			m_RenderTextures[i] = new VulkanTexture(m_Width, m_Height, m_AttachmentImages[i], m_AttachmentImageViews[i]);
		}
	}

}