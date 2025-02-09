#include "Renderer2DTestLayer.h"

#include <glm/gtc/matrix_transform.hpp>

void Renderer2DTestLayer::OnAttach()
{
	m_Renderer2D = new Atom::Renderer2D();
}

void Renderer2DTestLayer::OnDetach()
{
	delete m_Renderer2D;
}

void Renderer2DTestLayer::OnUpdate(float deltaTime)
{
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3{ 0.0f, 0.0f, 10.0f });
	view = glm::inverse(view);
	 
	m_Renderer2D->Begin(m_Projection, view);

	m_Renderer2D->SubmitQuad({ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });

	{
		const glm::vec2 position = { 0.0f, 0.0f };
		const float radius = 2.5f;

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { position.x, position.y, 0.0f }) * glm::scale(glm::mat4(1.0f), glm::vec3(radius));

		int segments = 128;
		for (int i = 0; i < segments; i++)
		{
			float angle = 2.0f * glm::pi<float>() * (float)i / segments;
			glm::vec4 startPosition = { glm::cos(angle), glm::sin(angle), 0.0f, 1.0f };
			angle = 2.0f * glm::pi<float>() * (float)((i + 1) % segments) / segments;
			glm::vec4 endPosition = { glm::cos(angle), glm::sin(angle), 0.0f, 1.0f };

			glm::vec3 p0 = transform * startPosition;
			glm::vec3 p1 = transform * endPosition;
			m_Renderer2D->SubmitLine(p0, p1, { 0.2f, 0.5f, 0.8f, 1.0f }, 0.075f);
		}
	}

	m_Renderer2D->End();

	// TODO: Render Fullscreen Quad, with RenderTexture of m_Renderer2D framebuffer
}

void Renderer2DTestLayer::OnImGui()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	if (ImGui::Begin("Renderer2D Viewport", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse))
	{
		const auto& viewportSize = ImGui::GetContentRegionAvail();
		if (viewportSize.x != m_ViewportSize.x || viewportSize.y != m_ViewportSize.y)
		{
			m_ViewportSize = { viewportSize.x, viewportSize.y };

			m_Projection = glm::perspective(glm::radians(45.0f), m_ViewportSize.x / m_ViewportSize.y, 0.1f, 1000.0f);

			m_Renderer2D->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
		}

		Atom::ImGui::Image(m_Renderer2D->GetOutput(), { m_ViewportSize.x, m_ViewportSize.y });
	}
	ImGui::End();
	ImGui::PopStyleVar(1);

	if (Atom::ImGui::BeginOverlay("Renderer2D Performance overlay"))
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Text("FPS: %.1f (%.3f ms/frame)", io.Framerate, 1000.0f / io.Framerate);

		ImGui::Separator();

		Atom::ImGui::DrawRenderer2DStats(m_Renderer2D->GetStatistics());
	}
	ImGui::End();
}
