#include "SandboxLayer.h"

void SandboxLayer::OnAttach()
{
	m_Shader = Atom::Shader::CreateFromFile("Assets/Shaders/static_vert.spv", "Assets/Shaders/static_frag.spv");

	Atom::PipelineOptions pipelineOptions{};
	pipelineOptions.ImageFormat = Atom::Enumerations::ImageFormat::B8G8R8A8_UNORM;
	pipelineOptions.Shader = m_Shader;
	m_Pipeline = Atom::Pipeline::Create(pipelineOptions);

	m_RenderCommand = Atom::RenderCommand::Create();

	Atom::Renderer::Initialize();
}

void SandboxLayer::OnDetach()
{
	Atom::Renderer::Shutdown();

	delete m_RenderCommand;
	m_RenderCommand = nullptr;

	delete m_Pipeline;
	m_Pipeline = nullptr;

	delete m_Shader;
	m_Shader = nullptr;
}

void SandboxLayer::OnUpdate(float deltaTime)
{
	Atom::Renderer::BeginFrame();

	Atom::Renderer::DrawStaticTriangle(m_Pipeline);

	Atom::Renderer::EndFrame();
}
