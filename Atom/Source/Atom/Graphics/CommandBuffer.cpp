#include "ATPCH.h"
#include "CommandBuffer.h"

#include "Atom/Graphics/Renderer.h"

#include "Atom/Graphics/Vulkan/VulkanCommandBuffer.h"

namespace Atom
{

	CommandBuffer* CommandBuffer::Create()
	{
		return Create(Renderer::GetFramesInFlight());
	}

	CommandBuffer* CommandBuffer::Create(uint32_t count)
	{
		return new VulkanCommandBuffer(count);
	}

	CommandBuffer::CommandBuffer(uint32_t count)
		: m_Count(count)
	{
	}

}