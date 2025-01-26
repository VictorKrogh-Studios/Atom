#pragma once
#include "Atom/Graphics/CommandBuffer.h"
#include "Atom/Graphics/UniformBuffer.h"
#include "Atom/Graphics/RenderPass.h"
#include "Atom/Graphics/Pipeline.h"
#include "Atom/Graphics/Shader.h"
#include "Atom/Graphics/VertexBuffer.h"
#include "Atom/Graphics/IndexBuffer.h"
#include "Atom/Graphics/StorageBuffer.h"

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

	struct Renderer2DStatistics
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;
		uint32_t LineCount = 0;
	};

	class Renderer2D
	{
	public:
		Renderer2D(const Renderer2DCapabilities& capabilities);
		~Renderer2D();

		void OnEvent(Event& event);

		void Begin(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
		void End();

		void SubmitQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		void SubmitQuad(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);

		void SubmitLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness = 0.02f);
		void SubmitLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float thickness = 0.02f);

		const Renderer2DStatistics& GetStatistics() const { return m_Statistics; }
	private:
		bool OnWindowResizeEvent(WindowResizeEvent& event);

		void StartBatch();
		void Flush();
		void NextBatch();
	private:
		Renderer2DCapabilities m_Capabilities;
		Renderer2DStatistics m_Statistics;
		CommandBuffer* m_CommandBuffer = nullptr;
		UniformBuffer* m_CameraUniformBuffer = nullptr;

		StorageBuffer* m_StorageBuffer = nullptr;

		// TODO: Move to a shader library
		Shader* m_LineShader = nullptr;
		Shader* m_QuadShader = nullptr;

		struct Renderer2DCameraUBO
		{
			glm::mat4 Projection;
			glm::mat4 View;
		};
		Renderer2DCameraUBO m_CameraUBO;

		glm::vec4 m_QuadVertexPositions[4];
		IndexBuffer* m_QuadIndexBuffer = nullptr;

		template<typename T>
		struct Pipeline2D
		{
			Pipeline* Pipeline = nullptr;
			RenderPass* RenderPass = nullptr;
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
			glm::vec4 VertexPosition;
			glm::vec4 Color;
			int32_t QuadIndex;
		};

		// For alignment issues; Use alignas(16) or extra padding members in the C++ struct.

		struct QuadTransformData
		{
			alignas(16) glm::vec3 Position;
			alignas(16) glm::vec3 Scale;
		};

		uint32_t m_QuadTransformDataCount = 0;
		QuadTransformData* m_QuadTransformDataBase;
		QuadTransformData* m_QuadTransformDataPtr;

		Pipeline2D<QuadVertex> m_QuadPipeline = {};

		Renderer2D::Pipeline2D<Renderer2D::QuadVertex> CreateQuadPipeline(Shader* shader);
		void DestroyQuadPipeline();

	private: // LINE VERTEX PIPELINE
		struct LineVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
		};

		Pipeline2D<LineVertex> m_LinePipeline = {};

		Renderer2D::Pipeline2D<Renderer2D::LineVertex> CreateLinePipeline(Shader* shader);
		void DestroyLinePipeline();
	};

}
