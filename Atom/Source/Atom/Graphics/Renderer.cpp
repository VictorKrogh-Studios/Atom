#include "ATPCH.h"
#include "Renderer.h"

#include "Atom/Graphics/SwapChain.h"

namespace Atom
{

	struct RendererData
	{
		RendererInitializeInfo InitializeInfo;
		uint32_t CurrentFrameIndex = 0;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Initialize(const RendererInitializeInfo& initializeInfo)
	{
		s_Data = new RendererData;
		s_Data->InitializeInfo = initializeInfo;
	}

	void Renderer::Shutdown()
	{
		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer::BeginFrame()
	{
		s_Data->InitializeInfo.SwapChain->BeginFrame();
	}

	void Renderer::EndFrame()
	{
		s_Data->CurrentFrameIndex = (s_Data->CurrentFrameIndex + 1) % s_Data->InitializeInfo.FramesInFlight;
	}

	void Renderer::PresentAndWait()
	{
		s_Data->InitializeInfo.SwapChain->Present();
	}

	uint32_t Renderer::GetCurrentFrameIndex()
	{
		return s_Data->CurrentFrameIndex;
	}

	uint32_t Renderer::GetFramesInFlight()
	{
		return s_Data->InitializeInfo.FramesInFlight;
	}

}