#pragma once
#include "Atom/Graphics/UniformBuffer.h"
#include "Atom/Graphics/Vulkan/Internal/VulkanBuffer.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanUniformBuffer : public UniformBuffer, public Internal::VulkanBuffer
	{
	public:
		VulkanUniformBuffer(uint64_t size, uint32_t framesInFlight);
		virtual ~VulkanUniformBuffer();

		virtual void Upload(void* data, uint64_t size, uint32_t frameIndex) const override;
	private:
		std::vector<VkBuffer> m_Buffers;
		std::vector<VkDeviceMemory> m_BufferMemories;
		std::vector<void*> m_BufferMemoriesMapped;

		friend class VulkanPipeline;
	};

}
