#include "ATPCH.h"
#include "VulkanDevice.h"

namespace Atom::Internal
{

	VulkanDevice* VulkanDevice::Create(VulkanPhysicalDevice* physicalDevice, VkPhysicalDeviceFeatures physicalDeviceFeatures)
	{
		return new VulkanDevice(physicalDevice, physicalDeviceFeatures);
	}

	VulkanDevice::VulkanDevice(VulkanPhysicalDevice* physicalDevice, VkPhysicalDeviceFeatures physicalDeviceFeatures)
		: m_PhysicalDevice(physicalDevice), m_PhysicalDeviceFeatures(physicalDeviceFeatures)
	{
		std::vector<const char*> deviceExtensions = GetDeviceExtensions();

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(m_PhysicalDevice->m_QueueCreateInfos.size());;
		deviceCreateInfo.pQueueCreateInfos = m_PhysicalDevice->m_QueueCreateInfos.data();
		deviceCreateInfo.pEnabledFeatures = &m_PhysicalDeviceFeatures;

		if (deviceExtensions.size() > 0)
		{
			deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
		}

		VkResult result = vkCreateDevice(m_PhysicalDevice->m_PhysicalDevice, &deviceCreateInfo, nullptr, &m_Device);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create logical device");

		vkGetDeviceQueue(m_Device, m_PhysicalDevice->m_QueueFamilyIndices.Graphics, 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_PhysicalDevice->m_QueueFamilyIndices.Transfer, 0, &m_TransferQueue);
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDeviceWaitIdle(m_Device);
		vkDestroyDevice(m_Device, nullptr);
	}

	std::vector<const char*> VulkanDevice::GetDeviceExtensions() const
	{
		// Do we need to enable any other extensions (eg. NV_RAYTRACING?)
		std::vector<const char*> deviceExtensions;
		// If the device will be used for presenting to a display via a swapchain we need to request the swapchain extension
		AT_CORE_ASSERT(m_PhysicalDevice->IsExtensionSupported(VK_KHR_SWAPCHAIN_EXTENSION_NAME));
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

#if AT_DEBUG
		if (m_PhysicalDevice->IsExtensionSupported(VK_EXT_DEBUG_MARKER_EXTENSION_NAME))
		{
			deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
		}
#endif

		return deviceExtensions;
	}

}