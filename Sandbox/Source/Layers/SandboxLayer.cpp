#include "SandboxLayer.h"

void SandboxLayer::OnAttach()
{
	m_Shader = Atom::Shader::CreateFromFile("Assets/Shaders/static_vert.spv", "Assets/Shaders/static_frag.spv");
}

void SandboxLayer::OnDetach()
{
	delete m_Shader;
	m_Shader = nullptr;
}

void SandboxLayer::OnUpdate(float deltaTime)
{
}
