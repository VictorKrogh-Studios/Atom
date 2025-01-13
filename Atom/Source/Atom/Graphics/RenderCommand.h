#pragma once

namespace Atom
{

	class Pipeline;
	
	class RenderCommand
	{
	public:
		static RenderCommand* Create();
	public:
		virtual ~RenderCommand() = default;

		virtual void BeginRecording() = 0;
		virtual void EndRecording() = 0;

		virtual void RenderStaticPipeline(Pipeline* pipeline, uint32_t vertexCount) = 0;
	};

}
