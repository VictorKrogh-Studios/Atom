#include "ATPCH.h"
#include "RenderPass.h"

#include "Atom/Graphics/Vulkan/VulkanRenderPass.h"

namespace Atom
{

	RenderPass* RenderPass::Create(const RenderPassCreateInfo& renderPassCreateInfo)
	{
		return new VulkanRenderPass(renderPassCreateInfo);
	}

	RenderPass::RenderPass(const RenderPassCreateInfo& renderPassCreateInfo)
		: m_CreateInfo(renderPassCreateInfo)
	{
	}

}