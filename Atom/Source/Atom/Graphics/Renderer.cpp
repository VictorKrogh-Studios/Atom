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

		RendererInitializeInfo InitializeInfo;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Initialize(const RendererInitializeInfo& initializeInfo)
	{
		s_Data = new RendererData;
		s_Data->InitializeInfo = initializeInfo;

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
	}

	void Renderer::EndFrame()
	{
	}

	void Renderer::BeginScene()
	{
		s_Data->RenderCommand->ResetCommandBuffer(s_Data->CommandBuffer);
		s_Data->RenderCommand->BeginCommandBuffer(s_Data->CommandBuffer);
	}

	void Renderer::EndScene()
	{
		s_Data->RenderCommand->EndCommandBuffer(s_Data->CommandBuffer);
	}

	void Renderer::DrawStaticTriangle(Pipeline* pipeline, uint32_t vertexCount)
	{
		s_Data->RenderCommand->RenderStaticPipeline(s_Data->CommandBuffer, pipeline, vertexCount);
	}

	uint32_t Renderer::GetFramesInFlight()
	{
		return s_Data->InitializeInfo.FramesInFlight;
	}

}