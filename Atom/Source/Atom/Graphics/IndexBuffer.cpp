#include "ATPCH.h"
#include "Atom/Graphics/IndexBuffer.h"

#include "Atom/Graphics/Vulkan/VulkanIndexBuffer.h"

namespace Atom
{

	IndexBuffer* IndexBuffer::Create(uint32_t count, uint32_t* indices)
	{
		return new VulkanIndexBuffer(count, indices);
	}

	IndexBuffer::IndexBuffer(uint32_t count)
		: m_Count(count)
	{
	}

}