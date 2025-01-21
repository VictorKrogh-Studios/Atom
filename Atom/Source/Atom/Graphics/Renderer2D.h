#pragma once
#include "Atom/Graphics/CommandBuffer.h"
#include "Atom/Graphics/UniformBuffer.h"
#include "Atom/Graphics/RenderPass.h"
#include "Atom/Graphics/Pipeline.h"
#include "Atom/Graphics/Shader.h"
#include "Atom/Graphics/VertexBuffer.h"
#include "Atom/Graphics/IndexBuffer.h"

#include "Atom/Events/WindowEvent.h"

#include <glm/glm.hpp>

namespace Atom
{

	struct Renderer2DCapabilities
	{
		Renderer2DCapabilities(uint32_t maxQuads = 1000)
			: MaxQuads(maxQuads), MaxVertices(maxQuads * 4), MaxIndices(maxQuads * 6)
		{
		}

	private:
		uint32_t MaxQuads = 1000;
		uint32_t MaxVertices;
		uint32_t MaxIndices;
		
		friend class Renderer2D;
	};

	class Renderer2D
	{
	public:
		Renderer2D(const Renderer2DCapabilities& capabilities);
		~Renderer2D();

		void OnEvent(Event& event);

		void Begin(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
		void End();

		void SubmitQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4& color);
		void SubmitQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color);
		void SubmitQuad(const glm::mat4& transform, const glm::vec4& color);
	private:
		bool OnWindowResizeEvent(WindowResizeEvent& event);

		void StartBatch();
		void Flush();
		void NextBatch();
	private:
		Renderer2DCapabilities m_Capabilities;
		CommandBuffer* m_CommandBuffer = nullptr;
		UniformBuffer* m_CameraUniformBuffer = nullptr;
		RenderPass* m_RenderPass = nullptr;

		struct Renderer2DCameraUBO
		{
			glm::mat4 Projection;
			glm::mat4 View;
		//	glm::mat4 Model = glm::mat4(1.0f);
		};
		Renderer2DCameraUBO m_CameraUBO;

		glm::vec4 m_QuadVertexPositions[4];
		IndexBuffer* m_QuadIndexBuffer = nullptr;

		template<typename T>
		struct Pipeline2D
		{
			Pipeline* Pipeline = nullptr;
			VertexBuffer* VertexBuffer = nullptr;
			Shader* Shader = nullptr;

			uint32_t IndexCount = 0;
			uint32_t PreviousIndexCount = 0;
			T* VertexBufferBase = nullptr;
			T* VertexBufferPtr = nullptr;
		};

	private: // QUAD VERTEX PIPELINE
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
		};

		Pipeline2D<QuadVertex> m_QuadPipeline = {};

		Renderer2D::Pipeline2D<Renderer2D::QuadVertex> CreateQuadPipeline();
		void DestroyQuadPipeline();
	};

}
