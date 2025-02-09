#include "ATPCH.h"
#include "VulkanSwapChain.h"
#include "VulkanGraphicsContext.h"

#include "VulkanUtils.h"

#include <GLFW/glfw3.h>

namespace Atom
{

	VulkanSwapChain* VulkanSwapChain::s_Instance = nullptr;

	VulkanSwapChain::VulkanSwapChain(const SwapChainOptions& options)
		: SwapChain(options)
	{
		s_Instance = this;

		VkPhysicalDevice physicalDevice = VulkanGraphicsContext::GetPhysicalDevice()->m_PhysicalDevice;
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		CreateSurface();

		FindSuitablePresentQueueIndex(physicalDevice);

		FindImageFormatAndColorSpace(physicalDevice);

		CreateRenderPass(device);

		CreateSwapChain(physicalDevice, device, nullptr);

		CreateImageViews(device);

		CreateFramebuffers(device);

		CreateSyncObjects(device);

		m_ImageFormat = Vulkan::Utils::GetImageFormat(m_ColorFormat);
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		VkInstance instance = VulkanGraphicsContext::GetVkInstance();
		VkDevice device = VulkanGraphicsContext::Get()->m_Device->m_Device;

		for (auto& framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		for (VkImageView imageView : m_SwapChainImageViews)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}

		m_SwapChainImageViews.clear();
		m_SwapChainImages.clear();

		vkDestroySwapchainKHR(device, m_SwapChain, nullptr);

		if (m_RenderPass)
		{
			vkDestroyRenderPass(device, m_RenderPass, nullptr);
		}

		vkDestroySurfaceKHR(instance, m_Surface, nullptr);

		for (auto& swapChainSemaphore : m_SwapChainSemaphores)
		{
			vkDestroySemaphore(device, swapChainSemaphore.ImageAvailableSemaphore, nullptr);
			vkDestroySemaphore(device, swapChainSemaphore.RenderFinishedSemaphore, nullptr);
		}
		m_SwapChainSemaphores.clear();

		for (auto& fence : m_Fences)
		{
			vkDestroyFence(device, fence, nullptr);
		}
		m_Fences.clear();

		vkDeviceWaitIdle(device);
	}

	void VulkanSwapChain::Resize(uint32_t width, uint32_t height)
	{
		VkPhysicalDevice physicalDevice = VulkanGraphicsContext::GetPhysicalDevice()->GetVkPhysicalDevice();
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		vkDeviceWaitIdle(device);

		VkSwapchainKHR oldSwapChain = m_SwapChain;

		//CleanupSwapChain(device);

		CreateSwapChain(physicalDevice, device, oldSwapChain);

		CreateImageViews(device);

		CreateFramebuffers(device);
	}

	uint32_t VulkanSwapChain::AquireNextImage(uint32_t frameIndex)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		VkResult result = vkWaitForFences(device, 1, &m_Fences[frameIndex], VK_TRUE, UINT64_MAX);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		result = vkResetFences(device, 1, &m_Fences[frameIndex]);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		result = vkAcquireNextImageKHR(device, m_SwapChain, UINT64_MAX, m_SwapChainSemaphores[frameIndex].ImageAvailableSemaphore, VK_NULL_HANDLE, &m_CurrentImageIndex);
		//result = vkAcquireNextImageKHR(device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[frameIndex], VK_NULL_HANDLE, &m_CurrentImageIndex);
		if (result != VK_SUCCESS)
		{
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				Resize(m_Width, m_Height);

				result = vkAcquireNextImageKHR(device, m_SwapChain, UINT64_MAX, m_SwapChainSemaphores[frameIndex].ImageAvailableSemaphore, VK_NULL_HANDLE, &m_CurrentImageIndex);
			}
		}

		return m_CurrentImageIndex;
	}

	void VulkanSwapChain::Present(uint32_t frameIndex, bool wait) 
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_SwapChain;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_SwapChainSemaphores[frameIndex].RenderFinishedSemaphore; // m_RenderFinishedSemaphores[frameIndex];
		presentInfo.pImageIndices = &m_CurrentImageIndex;
		VkResult result = vkQueuePresentKHR(VulkanGraphicsContext::GetDevice()->m_GraphicsQueue, &presentInfo);
		if (result != VK_SUCCESS)
		{
			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				Resize(m_Width, m_Height);
			}
			else
			{
				AT_CORE_ASSERT(result == VK_SUCCESS);
			}
		}

		if (wait)
		{
			vkDeviceWaitIdle(VulkanGraphicsContext::GetDevice()->m_Device);
		}
	}

	void VulkanSwapChain::CreateSurface()
	{
		VkInstance instance = VulkanGraphicsContext::GetVkInstance();

		VkResult result = glfwCreateWindowSurface(instance, m_Options.WindowHandle, nullptr, &m_Surface);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create surface");
	}

	// TODO: Move to VulkanPhysicalDevice
	void VulkanSwapChain::FindSuitablePresentQueueIndex(VkPhysicalDevice physicalDevice)
	{
		// Get available queue family properties
		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);
		AT_CORE_ASSERT(queueCount >= 1);

		std::vector<VkQueueFamilyProperties> queueProps(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());

		// Iterate over each queue to learn whether it supports presenting:
		// Find a queue with present support
		// Will be used to present the swap chain images to the windowing system
		std::vector<VkBool32> supportsPresent(queueCount);
		for (uint32_t i = 0; i < queueCount; i++)
		{
			vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, m_Surface, &supportsPresent[i]);
		}

		// Search for a graphics and a present queue in the array of queue
		// families, try to find one that supports both
		uint32_t graphicsQueueNodeIndex = UINT32_MAX;
		uint32_t presentQueueNodeIndex = UINT32_MAX;
		for (uint32_t i = 0; i < queueCount; i++)
		{
			if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				if (graphicsQueueNodeIndex == UINT32_MAX)
				{
					graphicsQueueNodeIndex = i;
				}

				if (supportsPresent[i] == VK_TRUE)
				{
					graphicsQueueNodeIndex = i;
					presentQueueNodeIndex = i;
					break;
				}
			}
		}
		if (presentQueueNodeIndex == UINT32_MAX)
		{
			// If there's no queue that supports both present and graphics
			// try to find a separate present queue
			for (uint32_t i = 0; i < queueCount; ++i)
			{
				if (supportsPresent[i] == VK_TRUE)
				{
					presentQueueNodeIndex = i;
					break;
				}
			}
		}

		AT_CORE_ASSERT(graphicsQueueNodeIndex != UINT32_MAX);
		AT_CORE_ASSERT(presentQueueNodeIndex != UINT32_MAX);

		m_QueueNodeIndex = graphicsQueueNodeIndex;
	}

	// TODO: Move to VulkanPhysicalDevice
	void VulkanSwapChain::FindImageFormatAndColorSpace(VkPhysicalDevice physicalDevice)
	{
		// Get list of supported surface formats
		uint32_t formatCount;
		VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, NULL);
		AT_CORE_ASSERT(result == VK_SUCCESS);
		AT_CORE_ASSERT(formatCount > 0);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_Surface, &formatCount, surfaceFormats.data());
		AT_CORE_ASSERT(result == VK_SUCCESS);

		// If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
		// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			m_ColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
			m_ColorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			// iterate over the list of available surface format and
			// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
			bool found_B8G8R8A8_UNORM = false;
			for (auto&& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					m_ColorFormat = surfaceFormat.format;
					m_ColorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}

			// in case VK_FORMAT_B8G8R8A8_UNORM is not available
			// select the first available color format
			if (!found_B8G8R8A8_UNORM)
			{
				m_ColorFormat = surfaceFormats[0].format;
				m_ColorSpace = surfaceFormats[0].colorSpace;
			}
		}
	}

	void VulkanSwapChain::CreateRenderPass(VkDevice device)
	{
		// Color attachment
		VkAttachmentDescription colorAttachmentDesc = {};
		colorAttachmentDesc.format = m_ColorFormat;
		colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthReference = {};
		depthReference.attachment = 1;
		depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachmentDesc;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_RenderPass);
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanSwapChain::CreateSwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSwapchainKHR oldSwapChain)
	{
		m_VSync = m_Options.VSync;

		// Get physical device surface properties and formats
		VkSurfaceCapabilitiesKHR surfCaps;
		VkResult result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &surfCaps);

		// TODO: Move to VulkanPhysicalDevice (make a method for retrieving Present Modes)
		// Get available present modes
		uint32_t presentModeCount;
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, NULL);
		AT_CORE_ASSERT(presentModeCount > 0);

		std::vector<VkPresentModeKHR> presentModes(presentModeCount);
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_Surface, &presentModeCount, presentModes.data());

		VkExtent2D swapchainExtent = {};
		// If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be set by the swapchain
		if (surfCaps.currentExtent.width == (uint32_t)-1)
		{
			// If the surface size is undefined, the size is set to
			// the size of the images requested.
			swapchainExtent.width = m_Options.Width;
			swapchainExtent.height = m_Options.Height;
		}
		else
		{
			// If the surface size is defined, the swap chain size must match
			swapchainExtent = surfCaps.currentExtent;
			m_Options.Width = surfCaps.currentExtent.width;
			m_Options.Height = surfCaps.currentExtent.height;
		}

		m_Width = m_Options.Width;
		m_Height = m_Options.Height;

		if (m_Width == 0 || m_Height == 0)
		{
			return;
		}

		// Select a present mode for the swapchain

		// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
		// This mode waits for the vertical blank ("v-sync")
		VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

		// If v-sync is not requested, try to find a mailbox mode
		// It's the lowest latency non-tearing present mode available
		if (!m_VSync)
		{
			for (size_t i = 0; i < presentModeCount; i++)
			{
				if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}
				if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
				{
					swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}
		}

		// Determine the number of images
		uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
		if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount))
		{
			desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
		}

		// Find the transformation of the surface
		VkSurfaceTransformFlagsKHR preTransform;
		if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			// We prefer a non-rotated transform
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			preTransform = surfCaps.currentTransform;
		}

		// Find a supported composite alpha format (not all devices support alpha opaque)
		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// Simply select the first composite alpha format available
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (auto& compositeAlphaFlag : compositeAlphaFlags)
		{
			if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag)
			{
				compositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = NULL;
		swapchainCreateInfo.surface = m_Surface;
		swapchainCreateInfo.minImageCount = desiredNumberOfSwapchainImages;
		swapchainCreateInfo.imageFormat = m_ColorFormat;
		swapchainCreateInfo.imageColorSpace = m_ColorSpace;
		swapchainCreateInfo.imageExtent = { swapchainExtent.width, swapchainExtent.height };
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = NULL;
		swapchainCreateInfo.presentMode = swapchainPresentMode;
		swapchainCreateInfo.oldSwapchain = oldSwapChain;
		// Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the surface area
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.compositeAlpha = compositeAlpha;

		// Enable transfer source on swap chain images if supported
		if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
		{
			swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}

		// Enable transfer destination on swap chain images if supported
		if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
		{
			swapchainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &m_SwapChain);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Swap Chain");

		if (oldSwapChain)
		{
			vkDestroySwapchainKHR(device, oldSwapChain, nullptr);
		}


		for (VkImageView imageView : m_SwapChainImageViews)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}

		m_SwapChainImageViews.clear();
		m_SwapChainImages.clear();

		result = vkGetSwapchainImagesKHR(device, m_SwapChain, &m_ImageCount, NULL);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		// Get the swap chain images
		m_SwapChainImages.resize(m_ImageCount);
		result = vkGetSwapchainImagesKHR(device, m_SwapChain, &m_ImageCount, m_SwapChainImages.data());
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanSwapChain::CreateImageViews(VkDevice device)
	{
		for (VkImageView imageView : m_SwapChainImageViews)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}
		m_SwapChainImageViews.clear();

		m_SwapChainImageViews.resize(m_ImageCount);
		for (size_t i = 0; i < m_ImageCount; i++)
		{
			VkImageViewCreateInfo colorAttachmentView = {};
			colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			colorAttachmentView.pNext = NULL;
			colorAttachmentView.format = m_ColorFormat;
			colorAttachmentView.image = m_SwapChainImages[i];
			colorAttachmentView.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};
			colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			colorAttachmentView.subresourceRange.baseMipLevel = 0;
			colorAttachmentView.subresourceRange.levelCount = 1;
			colorAttachmentView.subresourceRange.baseArrayLayer = 0;
			colorAttachmentView.subresourceRange.layerCount = 1;
			colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
			colorAttachmentView.flags = 0;

			VkResult result = vkCreateImageView(device, &colorAttachmentView, nullptr, &m_SwapChainImageViews[i]);
			AT_CORE_ASSERT(result == VK_SUCCESS);
		}
	}

	void VulkanSwapChain::CreateFramebuffers(VkDevice device)
	{
		for (auto& framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		VkFramebufferCreateInfo frameBufferCreateInfo = {};
		frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferCreateInfo.renderPass = m_RenderPass;
		frameBufferCreateInfo.attachmentCount = 1;
		frameBufferCreateInfo.width = m_Width;
		frameBufferCreateInfo.height = m_Height;
		frameBufferCreateInfo.layers = 1;

		m_Framebuffers.resize(m_ImageCount);
		for (uint32_t i = 0; i < m_Framebuffers.size(); i++)
		{
			frameBufferCreateInfo.pAttachments = &m_SwapChainImageViews[i];
			VkResult result = vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]);
			AT_CORE_ASSERT(result == VK_SUCCESS);
		}
	}

	void VulkanSwapChain::CreateSyncObjects(VkDevice device)
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		m_SwapChainSemaphores.resize(m_Options.FramesInFlight);
		for (uint32_t i = 0; i < m_Options.FramesInFlight; i++)
		{
			VkResult result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_SwapChainSemaphores[i].ImageAvailableSemaphore);
			AT_CORE_ASSERT(result == VK_SUCCESS);

			result = vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &m_SwapChainSemaphores[i].RenderFinishedSemaphore);
			AT_CORE_ASSERT(result == VK_SUCCESS);
		}

		if (m_Fences.size() != m_Options.FramesInFlight)
		{
			VkFenceCreateInfo fenceCreateInfo{};
			fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			m_Fences.resize(m_Options.FramesInFlight);
			for (auto& fence : m_Fences)
			{
				VkResult result = vkCreateFence(device, &fenceCreateInfo, nullptr, &fence);
				AT_CORE_ASSERT(result == VK_SUCCESS);
			}
		}
	}

}
