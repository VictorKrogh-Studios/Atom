#pragma once
#include "Atom/Graphics/CommandBuffer.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	namespace Internal
	{

		class VulkanPhysicalDevice;

	}

	class VulkanCommandBuffer : public CommandBuffer
	{
	public:
		VulkanCommandBuffer(uint32_t count);
		~VulkanCommandBuffer();
	private:
		void CreateCommandBuffer(VkDevice device);
	private:
		std::vector<VkCommandBuffer> m_CommandBuffers;

		friend class VulkanRenderCommand;
	};

}
