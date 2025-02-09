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
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };

private:
	class TexturePipeline
	{
	public:
		TexturePipeline(Atom::UniformBuffer* uniformBuffer, Atom::Texture* texture);
		~TexturePipeline();

		void Draw();
	private:
		Atom::RenderPass* m_RenderPass = nullptr;
		Atom::Shader* m_Shader = nullptr;
		Atom::Pipeline* m_Pipeline = nullptr;
		Atom::VertexBuffer* m_VertexBuffer = nullptr;
		Atom::IndexBuffer* m_IndexBuffer = nullptr;
		Atom::Texture* m_Texture = nullptr;
	private:
		struct Vertex_PositionColorTexCoord
		{
			glm::vec2 position;
			glm::vec3 color;
			glm::vec2 texCoord;
		};
		typedef Vertex_PositionColorTexCoord Vertex;

		const std::vector<Vertex> vertices = {
			{ {-0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f } },
			{ { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
			{ { 0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } },
			{ {-0.5f,  0.5f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } }
		};

		const std::vector<uint32_t> indices = { 
			0, 1, 2, 2, 3, 0
		};

		friend class SandboxLayer;
	};
	TexturePipeline* m_TexturePipeline = nullptr;
};

