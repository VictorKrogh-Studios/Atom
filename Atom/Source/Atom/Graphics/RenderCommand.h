#pragma once

namespace Atom
{

	class CommandBuffer;
	class Pipeline;

	class RenderCommand
	{
	public:
		static RenderCommand* Create();
	public:
		virtual ~RenderCommand() = default;

		virtual void ResetCommandBuffer(CommandBuffer* commandBuffer) = 0;
		virtual void BeginRecording(CommandBuffer* commandBuffer) = 0;
		virtual void EndRecording(CommandBuffer* commandBuffer) = 0;

		virtual void RenderStaticPipeline(CommandBuffer* commandBuffer, Pipeline* pipeline, uint32_t vertexCount) = 0;
	};

}
