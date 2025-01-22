#include "ATPCH.h"
#include "Atom/Graphics/CommandBuffer.h"

#include "Atom/Graphics/Renderer.h"

#include "Atom/Graphics/Vulkan/VulkanCommandBuffer.h"

namespace Atom
{

	CommandBuffer* CommandBuffer::Create(const CommandBufferCreateInfo& commandBufferCreateInfo)
	{
		return Create(commandBufferCreateInfo, Renderer::GetFramesInFlight());
	}

	CommandBuffer* CommandBuffer::Create(const CommandBufferCreateInfo& commandBufferCreateInfo, uint32_t count)
	{
		return new VulkanCommandBuffer(commandBufferCreateInfo, count);
	}

	CommandBuffer::CommandBuffer(const CommandBufferCreateInfo& commandBufferCreateInfo, uint32_t count)
		: m_CommandBufferCreateInfo(commandBufferCreateInfo), m_Count(count)
	{
	}

}