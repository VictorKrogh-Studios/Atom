#include "ATPCH.h"
#include "Atom/Graphics/UniformBuffer.h"
#include "Atom/Graphics/Renderer.h"

#include "Atom/Graphics/Vulkan/VulkanUniformBuffer.h"

namespace Atom
{

	UniformBuffer* UniformBuffer::Create(uint64_t size)
	{
		return new VulkanUniformBuffer(size, Renderer::GetFramesInFlight());
	}

	UniformBuffer::UniformBuffer(uint64_t size)
		: m_Size(size)
	{
	}

}