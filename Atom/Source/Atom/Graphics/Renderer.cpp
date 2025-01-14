#include "ATPCH.h"
#include "Renderer.h"

#include "Atom/Graphics/RenderCommand.h"
#include "Atom/Graphics/CommandBuffer.h"

namespace Atom
{

	struct RendererData
	{
		RenderCommand* RenderCommand;
		CommandBuffer* CommandBuffer;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Initialize()
	{
		s_Data = new RendererData;

		s_Data->RenderCommand = RenderCommand::Create();
		s_Data->CommandBuffer = CommandBuffer::Create();
	}

	void Renderer::Shutdown()
	{
		delete s_Data->RenderCommand;
		s_Data->RenderCommand = nullptr;

		delete s_Data->CommandBuffer;
		s_Data->CommandBuffer = nullptr;

		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer::BeginFrame()
	{
		s_Data->RenderCommand->ResetCommandBuffer(s_Data->CommandBuffer);
		s_Data->RenderCommand->BeginRecording(s_Data->CommandBuffer);
	}

	void Renderer::EndFrame()
	{
		s_Data->RenderCommand->EndRecording(s_Data->CommandBuffer);
	}

	void Renderer::DrawStaticTriangle(Pipeline* pipeline, uint32_t vertexCount)
	{
		s_Data->RenderCommand->RenderStaticPipeline(s_Data->CommandBuffer, pipeline, vertexCount);
	}

}