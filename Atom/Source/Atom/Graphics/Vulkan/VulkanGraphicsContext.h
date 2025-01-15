#pragma once
#include "Atom/Graphics/GraphicsContext.h"

#include "Internal/VulkanPhysicalDevice.h"
#include "Internal/VulkanDevice.h"

#include "Atom/Graphics/Vulkan/VulkanSwapChain.h"

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
		static Internal::VulkanPhysicalDevice* GetPhysicalDevice() { return Get()->m_PhysicalDevice; }
		static Internal::VulkanDevice* GetDevice() { return Get()->m_Device; }
		static VkInstance GetVkInstance() { return Get()->m_VkInstance; }

		void CreateVkInstance();
	private:
		static VulkanGraphicsContext* s_Instance;

		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugUtilsMessenger = VK_NULL_HANDLE;

		Internal::VulkanPhysicalDevice* m_PhysicalDevice;
		Internal::VulkanDevice* m_Device;

		friend class Atom::Internal::VulkanPhysicalDevice;
		friend class VulkanSwapChain;
		friend class VulkanShader;
		friend class VulkanPipeline;
		friend class VulkanRenderCommand;
		friend class VulkanCommandBuffer;
		friend class VulkanVertexBuffer;
	};

}
