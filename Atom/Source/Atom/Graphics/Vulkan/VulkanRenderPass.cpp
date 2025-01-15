#include "ATPCH.h"
#include "VulkanRenderPass.h"
#include "VulkanGraphicsContext.h"

#include "VulkanUtils.h"

namespace Atom
{

	VulkanRenderPass::VulkanRenderPass(const RenderPassCreateInfo& renderPassCreateInfo)
		: RenderPass(renderPassCreateInfo)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		CreateRenderPass(device);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		vkDestroyRenderPass(device, m_RenderPass, nullptr);
	}

	void VulkanRenderPass::CreateRenderPass(VkDevice device)
	{
		// Color attachment
		VkAttachmentDescription colorAttachmentDesc = {};
		colorAttachmentDesc.format = Vulkan::Utils::GetVkFormat(m_CreateInfo.ImageFormat); // m_ColorFormat;
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

		//VkAttachmentDescription colorAttachment{};
		//colorAttachment.format = Vulkan::Utils::GetVkFormat(m_CreateInfo.ImageFormat);
		//colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		//colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		//colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		//colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		//VkAttachmentReference colorAttachmentRef{};
		//colorAttachmentRef.attachment = 0;
		//colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		//VkSubpassDescription subpass{};
		//subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		//subpass.colorAttachmentCount = 1;
		//subpass.pColorAttachments = &colorAttachmentRef;

		//VkRenderPassCreateInfo renderPassInfo{};
		//renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		//renderPassInfo.attachmentCount = 1;
		//renderPassInfo.pAttachments = &colorAttachment;
		//renderPassInfo.subpassCount = 1;
		//renderPassInfo.pSubpasses = &subpass;

		VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_RenderPass);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass!");
	}

}