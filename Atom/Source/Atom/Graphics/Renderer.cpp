#include "ATPCH.h"
#include "Renderer.h"

#include "Atom/Graphics/SwapChain.h"
#include "Atom/Graphics/RenderCommand.h"

namespace Atom
{

	struct RendererData
	{
		RendererInitializeInfo InitializeInfo;
		uint32_t CurrentFrameIndex = 0;
		SwapChain* SwapChain;
		RenderCommand* RenderCommand;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Initialize(const RendererInitializeInfo& initializeInfo)
	{
		s_Data = new RendererData;
		s_Data->InitializeInfo = initializeInfo;
		s_Data->SwapChain = s_Data->InitializeInfo.SwapChain;
		s_Data->RenderCommand = RenderCommand::Create();
	}

	void Renderer::Shutdown()
	{
		delete s_Data->RenderCommand;
		s_Data->RenderCommand = nullptr;

		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer::BeginFrame()
	{
		s_Data->SwapChain->AquireNextImage(s_Data->CurrentFrameIndex);
	}

	void Renderer::EndFrame()
	{
	}

	void Renderer::PresentAndWait()
	{
		s_Data->SwapChain->Present(s_Data->CurrentFrameIndex, true);

		s_Data->CurrentFrameIndex = (s_Data->CurrentFrameIndex + 1) % s_Data->InitializeInfo.FramesInFlight;
	}

	uint32_t Renderer::GetCurrentFrameIndex()
	{
		return s_Data->CurrentFrameIndex;
	}

	uint32_t Renderer::GetFramesInFlight()
	{
		return s_Data->InitializeInfo.FramesInFlight;
	}

	RenderCommand* Renderer::GetRenderCommand()
	{
		return s_Data->RenderCommand;
	}

}