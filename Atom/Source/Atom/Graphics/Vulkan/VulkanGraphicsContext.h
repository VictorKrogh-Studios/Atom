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
		static VkInstance GetVkInstance() { return Get()->m_VkInstance; }

		static Internal::VulkanPhysicalDevice* GetPhysicalDevice() { return Get()->m_PhysicalDevice; }
		static Internal::VulkanDevice* GetDevice() { return Get()->m_Device; }

		void CreateVkInstance();
		VkCommandPool CreateCommandPool(uint32_t familyIndex);
	private:
		static VulkanGraphicsContext* s_Instance;

		VkInstance m_VkInstance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugUtilsMessenger = VK_NULL_HANDLE;

		Internal::VulkanPhysicalDevice* m_PhysicalDevice;
		Internal::VulkanDevice* m_Device;

		VkCommandPool m_GraphicsCommandPool = VK_NULL_HANDLE;
		VkCommandPool m_TransferCommandPool = VK_NULL_HANDLE;

		friend class Atom::Internal::VulkanPhysicalDevice;
		friend class VulkanSwapChain;
		friend class VulkanShader;
		friend class VulkanPipeline;
		friend class VulkanRenderCommand;
		friend class VulkanCommandBuffer;
		friend class VulkanRenderPass;

		friend class Atom::Internal::VulkanBuffer;
		friend class VulkanVertexBuffer;
		friend class VulkanIndexBuffer;
	};

}
