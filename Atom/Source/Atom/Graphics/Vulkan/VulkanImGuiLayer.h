#pragma once
#include "Atom/ImGui/ImGuiLayer.h"

#include "Atom/Graphics/Vulkan/VulkanRenderCommand.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanImGuiLayer : public ImGuiLayer
	{
	public:
		VulkanImGuiLayer(GLFWwindow* windowHandle, RenderCommand* renderCommand);
		virtual ~VulkanImGuiLayer() = default;
	protected:
		virtual void InitializeResources(uint32_t framesInFlight) override;
		virtual void ShutdownResources() override;

		virtual void OnBeginFrame() const override;
		virtual void OnRender(ImDrawData* draw_data, uint32_t frameIndex) const override;
	private:
		void CreateDescriptorPool(VkDevice device);
		void CreateCommandBuffers(VkDevice device, uint32_t framesInFlight);
		void CreateRenderPass(VkDevice device);
		void CreateWaitFences(VkDevice device, uint32_t framesInFlight);
	private:
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<VkFence> m_WaitFences;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;

		VulkanRenderCommand* m_RenderCommand = nullptr;
	};

}
