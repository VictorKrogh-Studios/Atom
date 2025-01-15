#include "SandboxLayer.h"

#include <glm/glm.hpp>

struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;
};

const std::vector<Vertex> vertices = {
	{ { 0.0f, -0.5f}, { 1.0f, 0.0f, 0.0f } },
	{ { 0.5f,  0.5f}, { 0.0f, 1.0f, 0.0f } },
	{ {-0.5f,  0.5f}, { 0.0f, 0.0f, 1.0f } }
};

void SandboxLayer::OnAttach()
{
	m_Shader = Atom::Shader::CreateFromFile("Assets/Shaders/shader_vert.spv", "Assets/Shaders/shader_frag.spv");

	Atom::PipelineOptions pipelineOptions{};
	pipelineOptions.ImageFormat = Atom::Enumerations::ImageFormat::B8G8R8A8_UNORM;
	pipelineOptions.Shader = m_Shader;
	pipelineOptions.Layout = {
		{ Atom::Enumerations::ShaderDataType::Float2, "inPosition" },
		{ Atom::Enumerations::ShaderDataType::Float3, "inColor" }
	};
	m_Pipeline = Atom::Pipeline::Create(pipelineOptions);

	m_VertexBuffer = Atom::VertexBuffer::Create(vertices.size() * sizeof(Vertex), (void*)vertices.data()); 

	m_Renderer = Atom::TestRenderer::Create();
}

void SandboxLayer::OnDetach()
{
	delete m_Renderer;
	m_Renderer = nullptr;

	delete m_VertexBuffer;
	m_VertexBuffer = nullptr;

	delete m_Pipeline;
	m_Pipeline = nullptr;

	delete m_Shader;
	m_Shader = nullptr;
}

void SandboxLayer::OnUpdate(float deltaTime)
{
	m_Renderer->BeginScene();

	//m_Renderer->DrawStaticTriangle(m_Pipeline);s
	m_Renderer->DrawVertices(m_Pipeline, m_VertexBuffer, vertices.size());

	m_Renderer->EndScene();
}
