#include "ATPCH.h"
#include "Renderer.h"

#include "Atom/Graphics/RenderCommand.h"
#include "Atom/Graphics/CommandBuffer.h"

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
		// TODO: Move Aquire swapchain image to here
	}

	void Renderer::EndFrame()
	{
		// TODO: Move presenting to here, or make a specific "Present" method for that?

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

}