#pragma once
#include "Atom/Graphics/CommandBuffer.h"
#include "Atom/Graphics/UniformBuffer.h"
#include "Atom/Graphics/RenderPass.h"
#include "Atom/Graphics/Pipeline.h"
#include "Atom/Graphics/Shader.h"
#include "Atom/Graphics/VertexBuffer.h"
#include "Atom/Graphics/IndexBuffer.h"
#include "Atom/Graphics/StorageBuffer.h"
#include "Atom/Graphics/Texture.h"
#include "Atom/Graphics/Framebuffer.h"

#include "Atom/Events/WindowEvent.h"

#include <glm/glm.hpp>

namespace Atom
{

	struct Renderer2DCapabilities
	{
		Renderer2DCapabilities(uint32_t maxQuads = 10000)
			: MaxQuads(maxQuads), MaxQuadVertices(maxQuads * 4), MaxQuadIndices(maxQuads * 6)
		{
		}

	private:
		uint32_t MaxQuads;
		uint32_t MaxQuadVertices;
		uint32_t MaxQuadIndices;

		friend class Renderer2D;
	};

	struct Renderer2DStatistics
	{
		uint32_t DrawCalls = 0;
		uint32_t QuadCount = 0;
		uint32_t LineCount = 0;

		uint32_t GetQuadVertexCount() const
		{
			return QuadCount * 4;
		}

		uint32_t GetQuadIndexCount() const
		{
			return QuadCount * 6;
		}

		uint32_t GetLineVertexCount() const
		{
			return LineCount * 4;
		}

		uint32_t GetLineIndexCount() const
		{
			return LineCount * 6;
		}
	};

	class Renderer2D
	{
	public:
		Renderer2D(const Renderer2DCapabilities& capabilities = Renderer2DCapabilities());
		~Renderer2D();

		void Resize(uint32_t width, uint32_t height);

		void Begin(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
		void End();

		void SubmitQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		void SubmitQuad(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);

		void SubmitQuad(const glm::vec2& position, const glm::vec2& size, Texture* texture, const glm::vec4& color = glm::vec4(1.0f));
		void SubmitQuad(const glm::vec3& position, const glm::vec3& size, Texture* texture, const glm::vec4& color = glm::vec4(1.0f));

		void SubmitLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness = 0.02f);
		void SubmitLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float thickness = 0.02f);

		Texture* GetOutput() const;

		const Renderer2DStatistics& GetStatistics() const
		{
			return m_Statistics;
		}
	private:
		struct QuadVertex
		{
			glm::vec4 VertexPosition;
			glm::vec4 Color;
			glm::vec2 TexCoord;
			int32_t TextureIndex;
			int32_t QuadIndex;
		};

		// For alignment issues; Use alignas(16) or extra padding members in the C++ struct.

		struct QuadTransformData
		{
			alignas(16) glm::vec3 Position;
			alignas(16) glm::vec3 Scale;
		};

		struct LineVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
		};
	private:
		void AddQuadVertexBuffer();
		Renderer2D::QuadVertex*& GetWriteableQuadBuffer();
		Renderer2D::QuadTransformData& GetQuadTransformDataPtr();

		void AddLineVertexBuffer();
		Renderer2D::LineVertex*& GetWriteableLinePtr();

		uint32_t GetTextureIndex(Texture* texture);

		void StartBatch();
		void Flush();
		void NextBatch();
	private:
		Renderer2DCapabilities m_Capabilities;
		Renderer2DStatistics m_Statistics;
		CommandBuffer* m_CommandBuffer = nullptr;
		UniformBuffer* m_CameraUniformBuffer = nullptr;
		Framebuffer* m_Framebuffer = nullptr;

		struct Renderer2DCameraUBO
		{
			glm::mat4 Projection;
			glm::mat4 View;
		};
		Renderer2DCameraUBO m_CameraUBO;

		glm::vec4 m_QuadVertexPositions[4];
		glm::vec2 m_QuadVertexTexCoords[4];

		Texture* m_WhiteTexture = nullptr;

	private:
		// QUAD VERTEX PIPELINE
		Pipeline* m_QuadPipeline = nullptr;
		RenderPass* m_QuadRenderPass = nullptr;
		IndexBuffer* m_QuadIndexBuffer = nullptr;

		std::vector<std::vector<VertexBuffer*>> m_QuadVertexBuffers;
		std::vector<std::vector<QuadVertex*>> m_QuadVertexBufferBases;
		std::vector<QuadVertex*> m_QuadVertexBufferPtr;
		uint32_t m_QuadIndexCount = 0;
		uint32_t m_QuadBufferWriteIndex = 0;

		uint32_t m_QuadTransformDataCount = 0;
		std::vector<QuadTransformData> m_QuadTransformDatas;
		StorageBuffer* m_QuadTransformDataStorageBuffer = nullptr;

		std::array<Texture*, 32> m_TextureSlots;
		uint32_t m_TextureSlotIndex = 1;			// 0 = White Texture

		// LINE VERTEX PIPELINE
		Pipeline* m_LinePipeline = nullptr;
		RenderPass* m_LineRenderPass = nullptr;
		IndexBuffer* m_LineIndexBuffer = nullptr;

		std::vector<std::vector<VertexBuffer*>> m_LineVertexBuffers;
		std::vector<std::vector<LineVertex*>> m_LineVertexBufferBases;
		std::vector<LineVertex*> m_LineVertexBufferPtr;
		uint32_t m_LineIndexCount = 0;
		uint32_t m_LineBufferWriteIndex = 0;
	};

}
