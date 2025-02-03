#include "ATPCH.h"
#include "VulkanRenderPass.h"
#include "VulkanGraphicsContext.h"
#include "VulkanSwapChain.h"
#include "VulkanFramebuffer.h"

#include "VulkanUtils.h"
#include "Internal/VulkanUtils.h"

namespace Atom
{

	VulkanRenderPass::VulkanRenderPass(const RenderPassCreateInfo& renderPassCreateInfo)
		: RenderPass(renderPassCreateInfo), m_RenderArea({ renderPassCreateInfo.RenderArea.x, renderPassCreateInfo.RenderArea.y })
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		// Hack
		if (m_CreateInfo.TargetSwapChain)
		{
			m_RenderPass = VulkanSwapChain::Get()->m_RenderPass;
			return;
		}

		CreateRenderPass(device);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		// Hack
		if (m_CreateInfo.TargetSwapChain)
		{
			return;
		}

		vkDestroyRenderPass(device, m_RenderPass, nullptr);
	}

	void VulkanRenderPass::Resize(uint32_t width, uint32_t height)
	{
		m_RenderArea = { width, height };

		// TODO: Not sure this is needed?
		if (m_Framebuffer)
		{
			m_Framebuffer->Resize(width, height);
		}
	}

	void VulkanRenderPass::SetRenderTarget(Framebuffer* framebuffer)
	{
		if (m_CreateInfo.TargetSwapChain)
		{
			AT_CORE_WARN("You are trying to set a render target, but TargetSwapChain is true");
			return;
		}

		m_Framebuffer = framebuffer;
	}

	VkFramebuffer VulkanRenderPass::GetVkFramebuffer() const
	{
		if (m_CreateInfo.TargetSwapChain)
		{
			return VulkanSwapChain::Get()->GetCurrentFramebuffer();
		}

		return static_cast<VulkanFramebuffer*>(m_Framebuffer)->GetVkFramebuffer(); 
	}

	void VulkanRenderPass::CreateRenderPass(VkDevice device)
	{
		const auto& attachments = m_CreateInfo.Attachments.Attachments;

		std::vector<VkAttachmentDescription> colorAttachmentDescriptions;
		colorAttachmentDescriptions.resize(attachments.size());

		std::vector<VkAttachmentReference> colorAttachmentReferences;
		colorAttachmentReferences.resize(attachments.size());

		for (uint32_t i = 0; i < colorAttachmentDescriptions.size(); i++)
		{
			VkAttachmentDescription& attachmentDescription = colorAttachmentDescriptions[i];

			attachmentDescription.format = Vulkan::Utils::GetVkFormat(attachments[i].Format);
			attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
			attachmentDescription.loadOp = Atom::Internal::VulkanUtils::GetVkAttachmentLoadOp(m_CreateInfo, attachments[i]);
			attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentDescription.initialLayout = attachmentDescription.loadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			attachmentDescription.finalLayout = Internal::VulkanUtils::GetVkImageLayout(attachments[i].FinalLayout);

			colorAttachmentReferences[i] = VkAttachmentReference{ i, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL };
		}

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = (uint32_t)colorAttachmentReferences.size();
		subpassDescription.pColorAttachments = colorAttachmentReferences.data();

		// TODO: do we need these?
		// Use subpass dependencies for layout transitions
		std::vector<VkSubpassDependency> dependencies;
		if (attachments.size())
		{
			{
				VkSubpassDependency& dependency = dependencies.emplace_back();
				dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
				dependency.dstSubpass = 0;
				dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			}
			{
				VkSubpassDependency& dependency = dependencies.emplace_back();
				dependency.srcSubpass = 0;
				dependency.dstSubpass = VK_SUBPASS_EXTERNAL;
				dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
				dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
				dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				dependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
			}
		}

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = (uint32_t)colorAttachmentDescriptions.size();
		renderPassInfo.pAttachments = colorAttachmentDescriptions.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

#if 0
		// Color attachment
		VkAttachmentDescription colorAttachmentDesc = {};
		colorAttachmentDesc.format = Vulkan::Utils::GetVkFormat(m_CreateInfo.ImageFormat); // m_ColorFormat;
		colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDesc.loadOp = Vulkan::Utils::GetVkAttachmentLoadOp(m_CreateInfo.LoadOperation); // VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDesc.initialLayout = m_CreateInfo.LoadOperation == Enumerations::RenderPassAttachmentLoadOperation::Clear ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // HACK
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
#endif

		VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_RenderPass);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass!");
	}

}