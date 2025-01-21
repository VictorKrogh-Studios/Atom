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
		VulkanVertexBuffer(const VertexBufferCreateInfo& createInfo);
		VulkanVertexBuffer(uint64_t size, void* vertices);
		virtual ~VulkanVertexBuffer();

		virtual void Upload(uint32_t size, void* vertices) const override;
	private:
		VertexBufferCreateInfo m_CreateInfo;
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;

		VkBuffer m_UploadBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_UploadBufferMemory = VK_NULL_HANDLE;

		friend class VulkanRenderCommand;
	};

}
