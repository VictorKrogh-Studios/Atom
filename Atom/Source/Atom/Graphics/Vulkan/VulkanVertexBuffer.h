#pragma once
#include "Atom/Graphics/VertexBuffer.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	namespace Internal
	{

		class VulkanPhysicalDevice;

	}

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(uint64_t size, void* vertices);
		~VulkanVertexBuffer();
	private:
		void CreateBuffer(VkDevice device, uint64_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
		void CopyBuffer(VkDevice device, VkBuffer srcBuffer, VkBuffer dstBuffer, uint64_t size) const;
	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;

		friend class VulkanRenderCommand;
	};

}
