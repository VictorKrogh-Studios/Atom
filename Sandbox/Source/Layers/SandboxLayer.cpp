#include "SandboxLayer.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

#include <imgui.h>

struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;
};

#ifdef USE_VERTICES
const std::vector<Vertex> vertices = {
	{ { 0.0f, -0.5f}, { 1.0f, 0.0f, 0.0f } },
	{ { 0.5f,  0.5f}, { 0.0f, 1.0f, 0.0f } },
	{ {-0.5f,  0.5f}, { 0.0f, 0.0f, 1.0f } }
};
#else
const std::vector<Vertex> vertices = {
	{ {-0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
	{ { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
	{ { 0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } },
	{ {-0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f } }
};

const std::vector<uint32_t> indices = {
	0, 1, 2, 2, 3, 0
};
#endif

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

static glm::mat4 s_Projection;

void SandboxLayer::OnAttach()
{
	m_UniformBuffer = Atom::UniformBuffer::Create(sizeof(UniformBufferObject));

	m_Shader = Atom::Shader::CreateFromFile("Assets/Shaders/shader_ubo_vert.spv", "Assets/Shaders/shader_ubo_frag.spv");

	Atom::RenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.ClearColor = { 0.2f, 0.5f, 0.8f, 1.0f };
	renderPassCreateInfo.ImageFormat = Atom::Application::Get().GetWindow()->GetImageFormat(); // Atom::Enumerations::ImageFormat::B8G8R8A8_UNORM;
	renderPassCreateInfo.RenderArea = { 1600, 900 };
	renderPassCreateInfo.TargetSwapChain = true;
	m_RenderPass = Atom::RenderPass::Create(renderPassCreateInfo);

	Atom::PipelineOptions pipelineOptions{};
	//pipelineOptions.ImageFormat = Atom::Enumerations::ImageFormat::B8G8R8A8_UNORM;
	pipelineOptions.RenderPass = m_RenderPass;
	pipelineOptions.Shader = m_Shader;
	pipelineOptions.Layout = {
		{ Atom::Enumerations::ShaderDataType::Float2, "inPosition" },
		{ Atom::Enumerations::ShaderDataType::Float3, "inColor" }
	};
	pipelineOptions.UniformBuffer = m_UniformBuffer;
	m_Pipeline = Atom::Pipeline::Create(pipelineOptions);

#ifdef USE_VERTICES
	m_VertexBuffer = Atom::VertexBuffer::Create(vertices.size() * sizeof(Vertex), (void*)vertices.data());
#else
	m_VertexBuffer = Atom::VertexBuffer::Create(vertices.size() * sizeof(Vertex), (void*)vertices.data());

	m_IndexBuffer = Atom::IndexBuffer::Create((uint32_t)indices.size(), (uint32_t*)indices.data());
#endif

	m_Renderer = Atom::TestRenderer::Create();

	m_Renderer2D = new Atom::Renderer2D({});

#if 0
	float m_OrthographicSize = 10.0f;
	float m_AspectRatio = 1600 / (float)900;

	float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
	float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
	float orthoBottom = -m_OrthographicSize * 0.5f;
	float orthoTop = m_OrthographicSize * 0.5f;
	s_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, -1.0f, 1.0f);
#else
	s_Projection = glm::perspective(glm::radians(45.0f), 1600 / (float)900, 0.1f, 100.0f);
	// projection[1][1] *= -1;
#endif
}

void SandboxLayer::OnDetach()
{
	delete m_Renderer2D;
	m_Renderer2D = nullptr;

	delete m_Renderer;
	m_Renderer = nullptr;

	delete m_VertexBuffer;
	m_VertexBuffer = nullptr;

	delete m_IndexBuffer;
	m_IndexBuffer = nullptr;

	delete m_UniformBuffer;
	m_UniformBuffer = nullptr;

	delete m_RenderPass;
	m_RenderPass = nullptr;

	delete m_Pipeline;
	m_Pipeline = nullptr;

	delete m_Shader;
	m_Shader = nullptr;
}

void SandboxLayer::OnUpdate(float deltaTime)
{
#if 1
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = s_Projection; // glm::perspective(glm::radians(45.0f), 1600 / (float)900, 0.1f, 10.0f);
	//ubo.proj[1][1] *= -1;

	m_UniformBuffer->Upload((void*)&ubo, sizeof(ubo), Atom::Renderer::GetCurrentFrameIndex());

	m_Renderer->BeginScene();

	m_Renderer->BeginRenderPass(m_RenderPass);

	//m_Renderer->DrawStaticTriangle(m_Pipeline);
#ifdef USE_VERTICES
	m_Renderer->DrawVertices(m_Pipeline, m_VertexBuffer, vertices.size());
#else
	m_Renderer->DrawIndexed(m_Pipeline, m_VertexBuffer, m_IndexBuffer, 6);
#endif

	m_Renderer->EndRenderPass();

	m_Renderer->EndScene();
#endif

	glm::mat4 view = glm::translate(glm::mat4(1.0f), m_CameraPosition); 
	//glm::mat4 view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	view = glm::inverse(view);

	m_Renderer2D->Begin(s_Projection, view);

	for (size_t i = 0; i < 900; i++)
	{
		m_Renderer2D->SubmitQuad({ 0.51f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f, 1.0f });
	}
	m_Renderer2D->SubmitQuad({ -0.51f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });

	m_Renderer2D->End();
}

void SandboxLayer::OnEvent(Atom::Event& event)
{
	m_Renderer2D->OnEvent(event);

	Atom::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<Atom::WindowResizeEvent>([this](Atom::WindowResizeEvent& e) { return OnWindowResizeEvent(e); });
}

void SandboxLayer::OnImGui()
{
	ImGui::ShowDemoWindow();

	if (ImGui::Begin("Debug"))
	{
		ImGui::DragFloat3("Camera Position", &m_CameraPosition[0], 0.1f);
	}
	ImGui::End();

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
	}
	ImGui::End();
}

bool SandboxLayer::OnWindowResizeEvent(Atom::WindowResizeEvent& event)
{
	s_Projection = glm::perspective(glm::radians(45.0f), event.GetWidth() / (float)event.GetHeight(), 0.1f, 100.0f);

	m_RenderPass->Resize(event.GetWidth(), event.GetHeight());
	return false;
}


