#include "ATPCH.h"
#include "ImGuiLayer.h"

#include "Atom/Core/Window.h"
#include "Atom/Core/Application.h"

#include "Atom/Graphics/Renderer.h"

#include <backends/imgui_impl_glfw.h>

#include <GLFW/glfw3.h>

#include "Atom/Graphics/Vulkan/VulkanImGuiLayer.h"

namespace Atom
{

	ImGuiLayer* ImGuiLayer::Create(Window* window, RenderCommand* renderCommand)
	{
		return new VulkanImGuiLayer(window->m_WindowHandle, renderCommand);
	}

	ImGuiLayer::ImGuiLayer(GLFWwindow* windowHandle)
		: Layer("ImGui Layer"), m_WindowHandle(windowHandle)
	{
	}

	void ImGuiLayer::OnAttach()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		//float fontSize = 18.0f;// *2.0f;
		//io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/OpenSans-Bold.ttf", fontSize);
		//io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/OpenSans-Regular.ttf", fontSize);

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, style.Colors[ImGuiCol_WindowBg].w);

		AT_CORE_ASSERT(ImGui_ImplGlfw_InitForVulkan(m_WindowHandle, true));

		InitializeResources(Renderer::GetFramesInFlight());

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		auto swapChain = Application::Get().GetWindow()->GetSwapChain();

		RenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.ImageFormat = Application::Get().GetWindow()->GetImageFormat();
		renderPassCreateInfo.ClearColor = { 0.1f, 0.1f,0.1f, 1.0f };
		renderPassCreateInfo.LoadOperation = Enumerations::RenderPassAttachmentLoadOperation::Load;
		renderPassCreateInfo.RenderArea = { swapChain->GetWidth(), swapChain->GetHeight() };
		renderPassCreateInfo.ImplicitSetViewport = false;
		renderPassCreateInfo.ImplicitSetScissor = false;
		renderPassCreateInfo.SubpassContents = Enumerations::RenderPassSubpassContents::SecondaryCommandBuffer;
		renderPassCreateInfo.TargetSwapChain = true;
		m_RenderPass = RenderPass::Create(renderPassCreateInfo);
	}

	void ImGuiLayer::OnDetach()
	{
		delete m_RenderPass;
		m_RenderPass = nullptr;

		ShutdownResources();

		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		OnBeginFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGui::Render();

		{
			uint32_t frameIndex = Renderer::GetCurrentFrameIndex();
			RenderCommand* renderCommand = Renderer::GetRenderCommand();
			CommandBuffer* drawCommandBuffer = Renderer::GetDrawCommandBuffer();

			renderCommand->BeginRenderPass(drawCommandBuffer, m_RenderPass, frameIndex);

			OnRender(ImGui::GetDrawData(), drawCommandBuffer, m_RenderPass, frameIndex);

			renderCommand->EndRenderPass(drawCommandBuffer, frameIndex);
		}

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

}