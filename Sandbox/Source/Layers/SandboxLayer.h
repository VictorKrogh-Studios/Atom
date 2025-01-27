#pragma once
#include <Atom.h>

class SandboxLayer : public Atom::Layer
{
public:
	SandboxLayer() : Atom::Layer("Sandbox Layer")
	{
	}
	virtual ~SandboxLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnEvent(Atom::Event& event) override;
	virtual void OnImGui() override;
private:
	bool OnWindowResizeEvent(Atom::WindowResizeEvent& event);
private:
	Atom::Shader* m_Shader = nullptr;
	Atom::RenderPass* m_RenderPass = nullptr;
	Atom::Pipeline* m_Pipeline = nullptr;
	Atom::VertexBuffer* m_VertexBuffer = nullptr;
	Atom::IndexBuffer* m_IndexBuffer = nullptr;
	Atom::UniformBuffer* m_UniformBuffer = nullptr;
	Atom::TestRenderer* m_Renderer = nullptr;
	Atom::Renderer2D* m_Renderer2D = nullptr;
	Atom::Texture* m_Texture = nullptr;

	glm::vec3 m_CameraPosition = { 0,0,10 };
};

