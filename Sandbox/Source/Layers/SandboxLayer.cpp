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

void SandboxLayer::OnAttach()
{
	m_UniformBuffer = Atom::UniformBuffer::Create(sizeof(UniformBufferObject));

	m_Shader = Atom::Shader::CreateFromFile("Assets/Shaders/shader_ubo_vert.spv", "Assets/Shaders/shader_ubo_frag.spv");

	Atom::RenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.ClearColor = { 0.2f, 0.5f, 0.8f, 1.0f };
	renderPassCreateInfo.ImageFormat = Atom::Application::Get().GetWindow()->GetImageFormat(); // Atom::Enumerations::ImageFormat::B8G8R8A8_UNORM;
	renderPassCreateInfo.RenderArea = { 1600, 900 };
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
}

void SandboxLayer::OnDetach()
{
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
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), 1600 / (float)900, 0.1f, 10.0f);
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
}

void SandboxLayer::OnImGui()
{
	ImGui::ShowDemoWindow();
}
