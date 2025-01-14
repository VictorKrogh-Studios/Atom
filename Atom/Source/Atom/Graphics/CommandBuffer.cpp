#include "ATPCH.h"
#include "CommandBuffer.h"

#include "Atom/Graphics/Vulkan/VulkanCommandBuffer.h"

namespace Atom
{

	CommandBuffer* CommandBuffer::Create()
	{
		return new VulkanCommandBuffer();
	}

}