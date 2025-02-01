#include "FractalTreeLayer.h"

#include <imgui.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static void DrawRenderer2DStats(const Atom::Renderer2DStatistics& stats)
{
	ImGui::Text("Renderer2D statistics:");
	ImGui::Text("Quad Count: %d", stats.QuadCount);
	ImGui::Text(" - Vertices: %d", stats.GetQuadVertexCount());
	ImGui::Text(" - Indices: %d", stats.GetQuadIndexCount());

	ImGui::Text("Line Count: %d", stats.LineCount);
	ImGui::Text(" - Vertices: %d", stats.GetLineVertexCount());
	ImGui::Text(" - Indices: %d", stats.GetLineIndexCount());

	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
}

void FractalTreeLayer::OnAttach()
{
	uint32_t windowWidth = Atom::Application::Get().GetWindow()->GetWidth();
	uint32_t windowHeight = Atom::Application::Get().GetWindow()->GetHeight();

	float m_OrthographicSize = 30.0f;
	float m_AspectRatio = windowWidth / (float)windowHeight;

	m_OrthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
	m_OrthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
	m_OrthoBottom = -m_OrthographicSize * 0.5f;
	m_OrthoTop = m_OrthographicSize * 0.5f;

	m_Projection = glm::ortho(m_OrthoLeft, m_OrthoRight, m_OrthoBottom, m_OrthoTop, -1.0f, 1.0f);
	m_View = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f });

	m_Renderer2D = new Atom::Renderer2D();

	m_AngleDegrees = glm::degrees(glm::pi<float>() / 4.0f);
}

void FractalTreeLayer::OnDetach()
{
	delete m_Renderer2D;
	m_Renderer2D = nullptr;
}

void FractalTreeLayer::OnUpdate(float deltaTime)
{
	if (Atom::Input::IsKeyPressed(AT_KEY_ESCAPE))
	{
		Atom::Application::Get().Close();
	}

	m_Renderer2D->Begin(m_Projection, m_View);

	DrawTree();

	m_Renderer2D->End();
}

void FractalTreeLayer::OnEvent(Atom::Event& event)
{
	m_Renderer2D->OnEvent(event);
}

void FractalTreeLayer::OnImGui()
{
	static int location = 1;
	ImGuiIO& io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	const float PAD = 10.0f;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
	ImVec2 work_size = viewport->WorkSize;
	ImVec2 window_pos, window_pos_pivot;
	window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
	window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
	window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
	window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
	ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	ImGui::SetNextWindowViewport(viewport->ID);
	window_flags |= ImGuiWindowFlags_NoMove;
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin("Example: Simple overlay", nullptr, window_flags))
	{
		ImGui::Text("FPS: %.1f (%.3f ms/frame)", io.Framerate, 1000.0f / io.Framerate);

		ImGui::Separator();

		DrawRenderer2DStats(m_Renderer2D->GetStatistics());

		ImGui::Separator();

		bool depthUpdated = ImGui::InputInt("Depth", (int32_t*) & m_Depth);
		bool lengthUpdated = ImGui::InputFloat("Trunk Length", &m_Length, 0.5f);
		bool angleUpdated = ImGui::InputFloat("Angle (Degrees)", &m_AngleDegrees, 0.5f);

		m_VariablesUpdated = depthUpdated || lengthUpdated || angleUpdated;
	}
	ImGui::End();
}

void FractalTreeLayer::DrawTree()
{
	DrawFractalTree({ 0.0f, -m_OrthoBottom }, -glm::radians(90.0f), m_Length, m_Depth);
}

void FractalTreeLayer::DrawFractalTree(glm::vec2 start, float angle, float length, uint32_t depth)
{
	if (depth == 0)
	{
		if (!m_VariablesUpdated)
		{
			return;
		}

		depth = m_Depth;
		m_VariablesUpdated = false;
	}

	glm::vec2 end = { start.x + length * glm::cos(angle), start.y + length * glm::sin(angle) };

	m_Renderer2D->SubmitLine(start, end, { 1.0f, 1.0f, 1.0f ,1.0f });

	float newLength = length * 0.7f;

	DrawFractalTree(end, angle - glm::radians(m_AngleDegrees), newLength, depth - 1);
	DrawFractalTree(end, angle + glm::radians(m_AngleDegrees), newLength, depth - 1);
}
