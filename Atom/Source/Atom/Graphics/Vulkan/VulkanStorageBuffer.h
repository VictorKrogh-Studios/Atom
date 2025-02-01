#pragma once
#include "Atom/Graphics/StorageBuffer.h"

#include "Internal/VulkanBuffer.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanStorageBuffer : public StorageBuffer, public Internal::VulkanBuffer
	{
	public:
		VulkanStorageBuffer(uint64_t size);
		virtual ~VulkanStorageBuffer();

		virtual void Upload(uint64_t size, void* data, uint32_t index) const override;
		virtual void Resize(uint64_t size, uint32_t index) override;
	private:
		std::vector<VkBuffer> m_Buffers;
		std::vector<VkDeviceMemory> m_BufferMemories;

		friend class VulkanPipeline;
		friend class VulkanShader;
	};

}
