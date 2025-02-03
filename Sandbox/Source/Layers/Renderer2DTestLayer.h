#pragma once
#include <Atom.h>

class Renderer2DTestLayer : public Atom::Layer
{
public:
	Renderer2DTestLayer() : Layer("Renderer2D Test Layer") { }
	virtual ~Renderer2DTestLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnImGui() override;
private:
	Atom::Renderer2D* m_Renderer2D = nullptr;
	glm::vec2 m_ViewportSize = {};

	glm::mat4 m_Projection = glm::mat4(1.0f);
	glm::mat4 m_View = glm::mat4(1.0f);
};
