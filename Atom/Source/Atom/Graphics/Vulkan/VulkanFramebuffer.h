#pragma once
#include "Atom/Graphics/Framebuffer.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferCreateInfo& createInfo);
		~VulkanFramebuffer();

		virtual void Resize(uint32_t width, uint32_t height) override;

		virtual RenderTexture* GetColorAttachment(uint32_t index) const override;

		VkFramebuffer GetVkFramebuffer() const { return m_Framebuffer; }
	private:
		void CreateImages(VkDevice device);
		void CreateImageViews(VkDevice device);
		void CreateFramebuffer(VkDevice device);
	private:
		VkFramebuffer m_Framebuffer = VK_NULL_HANDLE;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;

		std::vector<RenderTexture*> m_RenderTextures;

		std::vector<VkImage> m_AttachmentImages;
		VkImage m_DepthAttachmentImage = VK_NULL_HANDLE;

		std::vector<VkDeviceMemory> m_AttachmentImageMemories;
		VkDeviceMemory m_DepthAttachmentImageMemory = VK_NULL_HANDLE;

		std::vector<VkImageView> m_AttachmentImageViews;
		VkImageView m_DepthAttachmentImageView = VK_NULL_HANDLE;
	};

}
