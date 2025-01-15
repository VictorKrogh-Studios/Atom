#pragma once
#include "Atom/Graphics/VertexBuffer.h"
#include "Atom/Graphics/Vulkan/Internal/VulkanBuffer.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	namespace Internal
	{

		class VulkanPhysicalDevice;

	}

	class VulkanVertexBuffer : public VertexBuffer, public Internal::VulkanBuffer
	{
	public:
		VulkanVertexBuffer(uint64_t size, void* vertices);
		~VulkanVertexBuffer();
	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;

		friend class VulkanRenderCommand;
	};

}
