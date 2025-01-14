#include "SandboxLayer.h"

void SandboxLayer::OnAttach()
{
	m_Shader = Atom::Shader::CreateFromFile("Assets/Shaders/static_vert.spv", "Assets/Shaders/static_frag.spv");

	Atom::PipelineOptions pipelineOptions{};
	pipelineOptions.ImageFormat = Atom::Enumerations::ImageFormat::B8G8R8A8_UNORM;
	pipelineOptions.Shader = m_Shader;
	m_Pipeline = Atom::Pipeline::Create(pipelineOptions);
}

void SandboxLayer::OnDetach()
{
	delete m_Pipeline;
	m_Pipeline = nullptr;

	delete m_Shader;
	m_Shader = nullptr;
}

void SandboxLayer::OnUpdate(float deltaTime)
{
	Atom::Renderer::BeginScene();

	Atom::Renderer::DrawStaticTriangle(m_Pipeline);

	Atom::Renderer::EndScene();
}
