#include "ATPCH.h"
#include "Atom/Graphics/StorageBuffer.h"

#include "Atom/Graphics/Vulkan/VulkanStorageBuffer.h"

namespace Atom
{

	StorageBuffer* StorageBuffer::Create(uint64_t size)
	{
		return new VulkanStorageBuffer(size);
	}

	StorageBuffer::StorageBuffer(uint64_t size)
		: m_Size(size)
	{
	}

}