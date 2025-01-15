#pragma once

namespace Atom
{

	class CommandBuffer;
	class Pipeline;
	class VertexBuffer;
	class IndexBuffer;

	class RenderCommand
	{
	public:
		static RenderCommand* Create();
	public:
		virtual ~RenderCommand() = default;

		virtual void ResetCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const = 0;
		virtual void BeginCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const = 0;
		virtual void EndCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex) const = 0;
		virtual void SubmitCommandBuffer(CommandBuffer* commandBuffer, uint32_t frameIndex, bool wait = false) const = 0;

		virtual void RenderStaticPipeline(CommandBuffer* commandBuffer, Pipeline* pipeline, uint32_t vertexCount, uint32_t frameIndex) const = 0;
		virtual void DrawVertices(CommandBuffer* commandBuffer, Pipeline* pipeline, VertexBuffer* vertexBuffer, uint32_t vertexCount, uint32_t frameIndex) const = 0;
		virtual void DrawIndexed(CommandBuffer* commandBuffer, Pipeline* pipeline, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint32_t indexCount, uint32_t frameIndex) const = 0;
	};

}
