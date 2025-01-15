#pragma once
#include <Atom.h>


class SandboxLayer : public Atom::Layer
{
public:
	SandboxLayer() : Atom::Layer("Sandbox Layer") {}
	virtual ~SandboxLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float deltaTime) override;
private:
	Atom::Shader* m_Shader = nullptr;
	Atom::Pipeline* m_Pipeline = nullptr;
	Atom::TestRenderer* m_Renderer = nullptr;
};

