#pragma once
#include "Atom/Graphics/IndexBuffer.h"
#include "Atom/Graphics/Vulkan/Internal/VulkanBuffer.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanIndexBuffer : public IndexBuffer, public Internal::VulkanBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t count, uint32_t* indices);
		~VulkanIndexBuffer();
	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;

		friend class VulkanRenderCommand;
	};

}
