#pragma once
#include "Atom/Graphics/GraphicsContext.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanGraphicsContext : public GraphicsContext
	{
	public:
		VulkanGraphicsContext(const GraphicsContextOptions& options);
		virtual ~VulkanGraphicsContext();

	private:
		static VulkanGraphicsContext* Get() { return s_Instance; }
		static VkInstance GetVkInstance() { return Get()->m_VkInstance; }

		void CreateVkInstance();
	private:
		static VulkanGraphicsContext* s_Instance;

		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugUtilsMessenger = VK_NULL_HANDLE;

		VulkanPhysicalDevice* m_PhysicalDevice;
		VulkanDevice* m_Device;

		friend class VulkanPhysicalDevice;
		friend class VulkanSwapChain;
	};

}
