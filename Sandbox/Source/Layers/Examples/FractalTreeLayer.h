#pragma once
#include <Atom.h>

class FractalTreeLayer : public Atom::Layer
{
public:
	FractalTreeLayer() : Layer("Fractal Tree") {}
	virtual ~FractalTreeLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(float deltaTime) override;
	virtual void OnEvent(Atom::Event& event) override;
	virtual void OnImGui() override;
private:
	void DrawTree();
	void DrawFractalTree(glm::vec2 start, float angle, float length, uint32_t depth);
private:
	Atom::Renderer2D* m_Renderer2D = nullptr;

	float m_OrthoLeft = 0;
	float m_OrthoRight = 0;
	float m_OrthoBottom = 0;
	float m_OrthoTop = 0;
	glm::mat4 m_Projection = glm::mat4(1.0f);
	glm::mat4 m_View = glm::mat4(1.0f);

	uint32_t m_Depth = 15;
	float m_Length = 8.0f;
	float m_AngleDegrees = 0.0f;
	bool m_VariablesUpdated = false;
};


