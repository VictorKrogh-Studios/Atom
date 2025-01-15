#pragma once

#include "Atom/Graphics/RenderCommand.h"
#include "Atom/Graphics/CommandBuffer.h"
#include "Atom/Graphics/Pipeline.h"
#include "Atom/Graphics/VertexBuffer.h"
#include "Atom/Graphics/IndexBuffer.h"
#include "Atom/Graphics/RenderPass.h"

namespace Atom
{

	class TestRenderer
	{
	public:
		static TestRenderer* Create();
	public:
		TestRenderer();
		~TestRenderer();

		void BeginScene() const;
		void EndScene() const;

		void BeginRenderPass(RenderPass* renderPass) const;
		void EndRenderPass() const;

		void DrawStaticTriangle(Pipeline* pipeline);
		void DrawVertices(Pipeline* pipeline, VertexBuffer* vertexBuffer, uint32_t vertexCount);
		void DrawIndexed(Pipeline* pipeline, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint32_t indexCount);
	private:
		RenderCommand* m_RenderCommand = nullptr;
		CommandBuffer* m_CommandBuffer = nullptr;
	};

}
