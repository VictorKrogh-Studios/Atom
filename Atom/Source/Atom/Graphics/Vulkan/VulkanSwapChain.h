#pragma once
#include "Atom/Graphics/SwapChain.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanPhysicalDevice;

	class VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(const SwapChainOptions& options);
		~VulkanSwapChain();

		virtual uint32_t AquireNextImage(uint32_t frameIndex) const override;
		virtual void Present() override;
	private:
		static VulkanSwapChain* Get() { return s_Instance; }

		uint32_t AquireNextImage();

		void CreateSurface();
		void FindSuitablePresentQueueIndex(VkPhysicalDevice physicalDevice);
		void FindImageFormatAndColorSpace(VkPhysicalDevice physicalDevice);
		void CreateSwapChain(VkPhysicalDevice physicalDevice, VkDevice device);
		void CreateRenderPass(VkDevice device);
		void CreateFramebuffers(VkDevice device);
		void CreateSyncObjects(VkDevice device);
	private:
		static VulkanSwapChain* s_Instance;

		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		uint32_t m_ImageCount = 0;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		// Sync objects
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_Fences;

		VkRenderPass m_RenderPass;
		std::vector<VkFramebuffer> m_Framebuffers;

		uint32_t m_QueueNodeIndex = UINT32_MAX;

		VkFormat m_ColorFormat;
		VkColorSpaceKHR m_ColorSpace;

		uint32_t m_CurrentImageIndex = 0;
		uint32_t m_CurrentFrameIndex = 0;

		uint32_t m_Width;
		uint32_t m_Height;
		bool m_VSync = false;

		friend class VulkanRenderCommand;
		friend class VulkanPipeline;
	};

}
