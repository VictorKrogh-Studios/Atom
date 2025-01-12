#pragma once
#include "VulkanPhysicalDevice.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanDevice
	{
	public:
		static VulkanDevice* Create(VulkanPhysicalDevice* physicalDevice, VkPhysicalDeviceFeatures physicalDeviceFeatures);
	public:
		VulkanDevice(VulkanPhysicalDevice* physicalDevice, VkPhysicalDeviceFeatures physicalDeviceFeatures);
		~VulkanDevice();
	private:
		std::vector<const char*> GetDeviceExtensions() const;
	private:
		VulkanPhysicalDevice* m_PhysicalDevice;
		VkPhysicalDeviceFeatures m_PhysicalDeviceFeatures;

		VkDevice m_Device = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_TransferQueue = VK_NULL_HANDLE;

		friend class VulkanSwapChain;
		friend class VulkanShader;
	};

}