#pragma once
#include "Atom/Graphics/GraphicsContext.h"

#include "VulkanPhysicalDevice.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanGraphicsContext : public GraphicsContext
	{
	public:
		VulkanGraphicsContext(const GraphicsContextOptions& options);
		virtual ~VulkanGraphicsContext();

		static VkInstance GetVkInstance() { return s_Instance; }
	private:
		void CreateVkInstance();
	private:
		inline static VkInstance s_Instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugUtilsMessenger = VK_NULL_HANDLE;

		VulkanPhysicalDevice* m_PhysicalDevice;
	};

}
