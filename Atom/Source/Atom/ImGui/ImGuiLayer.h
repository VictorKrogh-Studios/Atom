#pragma once
#include "Atom/Core/Layer.h"

#include "Atom/Graphics/RenderCommand.h"
#include "Atom/Graphics/RenderPass.h"

#include <imgui.h>

struct GLFWwindow;

namespace Atom
{

	class Window;

	class ImGuiLayer : public Layer
	{
	public:
		static ImGuiLayer* Create(Window* window);
	public:
		ImGuiLayer(GLFWwindow* windowHandle);
		virtual ~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();
	protected:
		virtual void InitializeResources(uint32_t framesInFlight) = 0;
		virtual void ShutdownResources() = 0;

		virtual void OnBeginFrame() const = 0;
		virtual void OnRender(ImDrawData* draw_data, CommandBuffer* commandBuffer, RenderPass* renderPass, uint32_t frameIndex) const = 0;
	private:
		GLFWwindow* m_WindowHandle = nullptr;
		RenderPass* m_RenderPass = nullptr;
	};

}
