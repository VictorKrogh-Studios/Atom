#pragma once
#include <vulkan/vulkan.h>

namespace Atom::Internal
{

	class VulkanPhysicalDevice
	{
	public:
		struct QueueFamilyIndices
		{
			int32_t Graphics = -1;
			int32_t Compute = -1;
			int32_t Transfer = -1;
		};
	public:
		static VulkanPhysicalDevice* Select();
	public:
		VulkanPhysicalDevice();
		~VulkanPhysicalDevice() = default;

		bool IsExtensionSupported(const std::string& extension) const;

		const QueueFamilyIndices& GetQueueFamilyIndices() const { return m_QueueFamilyIndices; }
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

		VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }
	private:
		void FindSuitableGPU();
		QueueFamilyIndices GetQueueFamilyIndices(int queueFlags);
	private:
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceFeatures m_Features;
		VkPhysicalDeviceMemoryProperties m_MemoryProperties;

		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::unordered_set<std::string> m_SupportedExtensions;
		QueueFamilyIndices m_QueueFamilyIndices;
		std::vector<VkDeviceQueueCreateInfo> m_QueueCreateInfos;

		friend class VulkanDevice;
		friend class VulkanSwapChain;
		friend class VulkanRenderCommand;
		friend class VulkanCommandBuffer;
		friend class VulkanVertexBuffer;

		friend class VulkanImGuiLayer;
	};

}
