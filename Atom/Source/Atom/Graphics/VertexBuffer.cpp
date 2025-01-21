#include "ATPCH.h"
#include "VertexBuffer.h"

#include "Atom/Graphics/Vulkan/VulkanVertexBuffer.h"

namespace Atom
{

	VertexBuffer* VertexBuffer::Create(const VertexBufferCreateInfo& createInfo)
	{
		return new VulkanVertexBuffer(createInfo);
	}

	VertexBuffer* VertexBuffer::Create(uint64_t size, void* vertices)
	{
		return new VulkanVertexBuffer(size, vertices);
	}

}