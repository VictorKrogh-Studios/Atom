#include "ATPCH.h"
#include "VulkanImGuiLayer.h"
#include "VulkanGraphicsContext.h"

#include "Atom/Graphics/Vulkan/VulkanCommandBuffer.h"
#include "Atom/Graphics/Vulkan/VulkanRenderPass.h"

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

		bool result = ImGui_ImplVulkan_Init(&initInfo);
		AT_CORE_ASSERT(result);
	}

	void VulkanImGuiLayer::ShutdownResources()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		ImGui_ImplVulkan_Shutdown();

		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
	}

	void VulkanImGuiLayer::OnBeginFrame() const
	{
		ImGui_ImplVulkan_NewFrame();
	}

	void VulkanImGuiLayer::OnRender(ImDrawData* draw_data, CommandBuffer* commandBuffer, RenderPass* renderPass, uint32_t frameIndex) const
	{
		VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
		VulkanRenderPass* vulkanRenderPass = static_cast<VulkanRenderPass*>(renderPass);

		VulkanSwapChain* swapChain = VulkanSwapChain::Get();

		uint32_t width = swapChain->m_Width;
		uint32_t height = swapChain->m_Height;

		VkCommandBuffer drawCommandBuffer = vulkanCommandBuffer->m_CommandBuffers[frameIndex]; 

		VkCommandBufferInheritanceInfo inheritanceInfo = {};
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.renderPass = vulkanRenderPass->GetVkRenderPass();
		inheritanceInfo.framebuffer = vulkanRenderPass->GetVkFramebuffer();

		VkCommandBufferBeginInfo cmdBufInfo = {};
		cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		cmdBufInfo.pInheritanceInfo = &inheritanceInfo;

		VkResult result = vkBeginCommandBuffer(m_CommandBuffers[frameIndex], &cmdBufInfo);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = (float)height;
		viewport.height = -(float)height;
		viewport.width = (float)width;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_CommandBuffers[frameIndex], 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = width;
		scissor.extent.height = height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;
		vkCmdSetScissor(m_CommandBuffers[frameIndex], 0, 1, &scissor);

		ImGui_ImplVulkan_RenderDrawData(draw_data, m_CommandBuffers[frameIndex]);

		vkEndCommandBuffer(m_CommandBuffers[frameIndex]);

		vkCmdExecuteCommands(drawCommandBuffer, 1, &m_CommandBuffers[frameIndex]);
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
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;

		VkResult result = vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, m_CommandBuffers.data());
		AT_CORE_ASSERT(result == VK_SUCCESS);
	}

}