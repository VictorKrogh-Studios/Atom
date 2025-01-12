#pragma once
#include <Atom.h>


class SandboxLayer : public Atom::Layer
{
public:
	SandboxLayer() : Atom::Layer("Sandbox Layer") {}
	virtual ~SandboxLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
private:
	Atom::Shader* m_Shader = nullptr;
	Atom::Pipeline* m_Pipeline = nullptr;
};

