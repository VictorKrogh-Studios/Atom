#include "ATPCH.h"
#include "VulkanImGuiLayer.h"
#include "VulkanGraphicsContext.h"

#include <backends/imgui_impl_vulkan.h>

namespace Atom
{

	VulkanImGuiLayer::VulkanImGuiLayer(GLFWwindow* windowHandle)
		: ImGuiLayer(windowHandle)
	{
	}

	void VulkanImGuiLayer::InitializeResources(uint32_t framesInFlight)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();
		VulkanSwapChain* swapChain = VulkanSwapChain::Get();

		CreateDescriptorPool(device);

		CreateCommandBuffers(device, framesInFlight);

		CreateRenderPass(device);

		CreateWaitFences(device, framesInFlight);

		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = VulkanGraphicsContext::GetVkInstance();
		initInfo.PhysicalDevice = VulkanGraphicsContext::GetPhysicalDevice()->GetVkPhysicalDevice();
		initInfo.Device = VulkanGraphicsContext::GetDevice()->GetVkDevice();
		initInfo.QueueFamily = VulkanGraphicsContext::GetPhysicalDevice()->m_QueueFamilyIndices.Graphics;
		initInfo.Queue = VulkanGraphicsContext::GetDevice()->m_GraphicsQueue;
		initInfo.DescriptorPool = m_DescriptorPool;
		initInfo.MinImageCount = 2;
		initInfo.ImageCount = swapChain->m_ImageCount;
		initInfo.RenderPass = swapChain->m_RenderPass;

		AT_CORE_ASSERT(ImGui_ImplVulkan_Init(&initInfo));
	}

	void VulkanImGuiLayer::ShutdownResources()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		ImGui_ImplVulkan_Shutdown();

		vkDestroyRenderPass(device, m_RenderPass, nullptr);

		for (size_t i = 0; i < m_WaitFences.size(); i++)
		{
			vkDestroyFence(device, m_WaitFences[i], nullptr);
		}

		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
	}

	void VulkanImGuiLayer::OnBeginFrame() const
	{
		ImGui_ImplVulkan_NewFrame();
	}

	void VulkanImGuiLayer::OnRender(ImDrawData* draw_data, uint32_t frameIndex) const
	{
		VulkanSwapChain* swapChain = VulkanSwapChain::Get();

		VkCommandBufferInheritanceInfo inheritanceInfo = {};
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.renderPass = swapChain->m_RenderPass;
		inheritanceInfo.framebuffer = swapChain->m_Framebuffers[swapChain->m_CurrentImageIndex];

		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		//cmdBufInfo.pInheritanceInfo = &inheritanceInfo;

		VkResult result = vkBeginCommandBuffer(m_CommandBuffers[frameIndex], &cmdBufInfo);

		VkRenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = new VkClearValue({ 0.1f, 0.1f, 0.1f, 1.0f });
		renderPassBeginInfo.renderPass = m_RenderPass;
		renderPassBeginInfo.framebuffer = swapChain->m_Framebuffers[swapChain->m_CurrentImageIndex];
		renderPassBeginInfo.renderArea.offset = { 0,0 };
		renderPassBeginInfo.renderArea.extent = { 1600, 900 };
		vkCmdBeginRenderPass(m_CommandBuffers[frameIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		ImGui_ImplVulkan_RenderDrawData(draw_data, m_CommandBuffers[frameIndex]);

		vkCmdEndRenderPass(m_CommandBuffers[frameIndex]);

		vkEndCommandBuffer(m_CommandBuffers[frameIndex]);

		{
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pCommandBuffers = &m_CommandBuffers[frameIndex];
			submitInfo.commandBufferCount = 1;

			vkWaitForFences(VulkanGraphicsContext::GetDevice()->GetVkDevice(), 1, &m_WaitFences[frameIndex], VK_TRUE, UINT64_MAX);
			vkResetFences(VulkanGraphicsContext::GetDevice()->GetVkDevice(), 1, &m_WaitFences[frameIndex]);

			VkResult result = vkQueueSubmit(VulkanGraphicsContext::GetDevice()->m_GraphicsQueue, 1, &submitInfo, m_WaitFences[frameIndex]);
			AT_CORE_ASSERT(result == VK_SUCCESS);
		}
	}

	void VulkanImGuiLayer::CreateDescriptorPool(VkDevice device)
	{
		// Create Descriptor Pool
		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 100 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 100 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 100 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 100 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 100 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 100 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 100 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		VkResult result = vkCreateDescriptorPool(device, &pool_info, nullptr, &m_DescriptorPool);
	}

	void VulkanImGuiLayer::CreateCommandBuffers(VkDevice device, uint32_t framesInFlight)
	{
		m_CommandBuffers.resize(framesInFlight);

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandBufferCount = framesInFlight;
		commandBufferAllocateInfo.commandPool = VulkanGraphicsContext::Get()->m_GraphicsCommandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

		VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, m_CommandBuffers.data());
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

	void VulkanImGuiLayer::CreateRenderPass(VkDevice device)
	{
		// Color attachment
		VkAttachmentDescription colorAttachmentDesc = {};
		colorAttachmentDesc.format = VulkanSwapChain::Get()->m_ColorFormat;
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
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass!");
	}

	void VulkanImGuiLayer::CreateWaitFences(VkDevice device, uint32_t framesInFlight)
	{
		m_WaitFences.resize(framesInFlight);

		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (uint32_t i = 0; i < framesInFlight; i++)
		{
			vkCreateFence(device, &fenceCreateInfo, nullptr, &m_WaitFences[i]);
		}
	}

}