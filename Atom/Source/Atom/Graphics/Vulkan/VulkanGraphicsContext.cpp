#include "ATPCH.h"
#include "VulkanGraphicsContext.h"
#include "Internal/VulkanPhysicalDevice.h"

namespace Atom
{
	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugUtilsMessengerCallback(const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, const VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		switch (messageSeverity)
		{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: AT_CORE_TRACE(pCallbackData->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: AT_CORE_INFO(pCallbackData->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: AT_CORE_WARN(pCallbackData->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: AT_CORE_ERROR(pCallbackData->pMessage); break;
			default: break;
		}

		return VK_FALSE;
	}

	static bool CheckDriverAPIVersionSupport(uint32_t minimumSupportedVersion)
	{
		uint32_t instanceVersion;
		vkEnumerateInstanceVersion(&instanceVersion);

		if (instanceVersion < minimumSupportedVersion)
		{
			AT_CORE_CRITICAL("Incompatible Vulkan driver version!");
			AT_CORE_CRITICAL("  You have {}.{}.{}", VK_API_VERSION_MAJOR(instanceVersion), VK_API_VERSION_MINOR(instanceVersion), VK_API_VERSION_PATCH(instanceVersion));
			AT_CORE_CRITICAL("  You need at least {}.{}.{}", VK_API_VERSION_MAJOR(minimumSupportedVersion), VK_API_VERSION_MINOR(minimumSupportedVersion), VK_API_VERSION_PATCH(minimumSupportedVersion));

			return false;
		}

		return true;
	}

	VulkanGraphicsContext* VulkanGraphicsContext::s_Instance = nullptr;

	VulkanGraphicsContext::VulkanGraphicsContext(const GraphicsContextOptions& options)
		: GraphicsContext(options)
	{
		s_Instance = this;

		if (!CheckDriverAPIVersionSupport(VK_API_VERSION_1_2))
		{
			AT_CORE_ERROR("Incompatible Vulkan driver version.\nUpdate your GPU drivers!");
			AT_CORE_ASSERT(true);
		}

		CreateVkInstance();

		m_PhysicalDevice = Internal::VulkanPhysicalDevice::Select();

		VkPhysicalDeviceFeatures enabledPhysicalDeviceFeatures;
		memset(&enabledPhysicalDeviceFeatures, 0, sizeof(VkPhysicalDeviceFeatures));
		enabledPhysicalDeviceFeatures.samplerAnisotropy = true; 
		enabledPhysicalDeviceFeatures.wideLines = true; 
		enabledPhysicalDeviceFeatures.fillModeNonSolid = true;
		enabledPhysicalDeviceFeatures.independentBlend = true; 
		enabledPhysicalDeviceFeatures.pipelineStatisticsQuery = true; 
		enabledPhysicalDeviceFeatures.shaderStorageImageReadWithoutFormat = true; 
		m_Device = Internal::VulkanDevice::Create(m_PhysicalDevice, enabledPhysicalDeviceFeatures);

		m_GraphicsCommandPool = CreateCommandPool(m_PhysicalDevice->GetQueueFamilyIndices().Graphics);
		m_TransferCommandPool = CreateCommandPool(m_PhysicalDevice->GetQueueFamilyIndices().Transfer);
	}

	VulkanGraphicsContext::~VulkanGraphicsContext()
	{
		vkDestroyCommandPool(m_Device->GetVkDevice(), m_TransferCommandPool, nullptr);
		vkDestroyCommandPool(m_Device->GetVkDevice(), m_GraphicsCommandPool, nullptr);

		delete m_Device;
		m_Device = nullptr;

		delete m_PhysicalDevice;
		m_PhysicalDevice = nullptr;

		if (m_Options.EnableValidation)
		{
			auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
			vkDestroyDebugUtilsMessengerEXT(m_VkInstance, m_DebugUtilsMessenger, nullptr);
			m_DebugUtilsMessenger = VK_NULL_HANDLE;
		}
		vkDestroyInstance(m_VkInstance, nullptr);
		m_VkInstance = VK_NULL_HANDLE;
	}

	void VulkanGraphicsContext::CreateVkInstance()
	{

		VkApplicationInfo applicationInfo{};
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pApplicationName = "";
		applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.pEngineName = "Atom";
		applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		applicationInfo.apiVersion = VK_API_VERSION_1_2;

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &applicationInfo;
		instanceCreateInfo.enabledExtensionCount = m_Options.GlfwExtensionsCount;
		instanceCreateInfo.ppEnabledExtensionNames = m_Options.GlfwExtensions;

		std::vector<const char*> extensions(m_Options.GlfwExtensions, m_Options.GlfwExtensions + m_Options.GlfwExtensionsCount);
		if (m_Options.EnableValidation)
		{
			const char* validationLayerName = "VK_LAYER_KHRONOS_validation";

			uint32_t instanceLayerCount;
			vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
			std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
			vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());

			bool validationLayerPresent = false;
			AT_CORE_INFO("Vulkan Instance Layers:");
			for (const VkLayerProperties& layer : instanceLayerProperties)
			{
				AT_CORE_INFO("Renderer: {0}", layer.layerName);
				if (strcmp(layer.layerName, validationLayerName) == 0)
				{
					validationLayerPresent = true;
					break;
				}
			}
			if (validationLayerPresent)
			{
				extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

				instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
				instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

				instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
				instanceCreateInfo.enabledLayerCount = 1;
			}
			else
			{
				AT_CORE_ERROR("Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled");
			}
		}

		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_VkInstance);

		if (m_Options.EnableValidation)
		{
			auto vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugUtilsMessengerEXT");
			AT_CORE_ASSERT(vkCreateDebugUtilsMessengerEXT != NULL, "");
			VkDebugUtilsMessengerCreateInfoEXT debugUtilsCreateInfo{};
			debugUtilsCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugUtilsCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugUtilsCreateInfo.pfnUserCallback = VulkanDebugUtilsMessengerCallback;
			debugUtilsCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;

			vkCreateDebugUtilsMessengerEXT(m_VkInstance, &debugUtilsCreateInfo, nullptr, &m_DebugUtilsMessenger);
		}
	}

	VkCommandPool VulkanGraphicsContext::CreateCommandPool(uint32_t queueFamilyIndex)
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex; 

		VkCommandPool commandPool = VK_NULL_HANDLE;
		VkResult result = vkCreateCommandPool(m_Device->GetVkDevice(), &commandPoolCreateInfo, nullptr, &commandPool);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		return commandPool;
	}

}