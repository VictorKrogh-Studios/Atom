#include "FractalTreeLayer.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void FractalTreeLayer::OnAttach()
{
	uint32_t windowWidth = Atom::Application::Get().GetWindow()->GetWidth();
	uint32_t windowHeight = Atom::Application::Get().GetWindow()->GetHeight();

	float m_OrthographicSize = 20.0f;
	float m_AspectRatio = windowWidth / (float)windowHeight;

	m_OrthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
	m_OrthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
	m_OrthoBottom = -m_OrthographicSize * 0.5f;
	m_OrthoTop = m_OrthographicSize * 0.5f;

	m_Projection = glm::ortho(m_OrthoLeft, m_OrthoRight, m_OrthoBottom, m_OrthoTop, -1.0f, 1.0f);
	m_View = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f });

	m_Renderer2D = new Atom::Renderer2D({});
}

void FractalTreeLayer::OnDetach()
{
	delete m_Renderer2D;
	m_Renderer2D = nullptr;
}

static uint32_t count = 0;
static uint32_t prev_count = 0;

void FractalTreeLayer::OnUpdate(float deltaTime)
{
	if (Atom::Input::IsKeyPressed(AT_KEY_ESCAPE))
	{
		Atom::Application::Get().Close();
	}

	m_Renderer2D->Begin(m_Projection, m_View);
	uint32_t len = 9.0f;

	m_Renderer2D->SubmitQuad({ 0.0f, -m_OrthoBottom }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });

	count = 0;

	Branch({ 0.0f, -m_OrthoBottom }, len, 0.0f);

	if (prev_count != count)
	{
		AT_TRACE(count);
		prev_count = count;
	}

	m_Renderer2D->End();
}

void FractalTreeLayer::OnEvent(Atom::Event& event)
{
	m_Renderer2D->OnEvent(event);
}

void FractalTreeLayer::OnImGui()
{
}

static glm::vec2 CalculateEndPosition(const glm::vec2& start, float length, float angle)
{
	return { start.x + length * glm::cos(angle), start.y + length * glm::sin(angle) };
}

void FractalTreeLayer::Branch(glm::vec2 startPosition, float length, float angle)
{
	count++;
	glm::vec2 endPosition = CalculateEndPosition(startPosition, length, angle) - length;

	m_Renderer2D->SubmitQuad(startPosition, { 0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f, 1.0f });
	m_Renderer2D->SubmitLine(startPosition, endPosition, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.03f);
	//m_Renderer2D->SubmitLine(startPosition, { 0.0f, -m_OrthoBottom - length }, { 1.0f, 1.0f, 1.0f, 1.0f }, 0.05f);

	if (length > 2.5f)
	{
		Branch(endPosition, length * 0.77f, glm::pi<float>() / 4.0f);
		// m_Renderer2D->SubmitLine(endPosition, CalculateEndPosition(endPosition, length * 0.67f, -glm::pi<float>() / 4.0f), { 1.0f, 1.0f, 1.0f, 1.0f }, 0.04f);
	}
}
