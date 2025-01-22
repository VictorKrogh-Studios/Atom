#include "ATPCH.h"
#include "Atom/Graphics/Renderer.h"

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
		CommandBuffer* DrawCommandBuffers;
	};

	static RendererData* s_Data = nullptr;

	void Renderer::Initialize(const RendererInitializeInfo& initializeInfo)
	{
		s_Data = new RendererData;
		s_Data->InitializeInfo = initializeInfo;
		s_Data->SwapChain = s_Data->InitializeInfo.SwapChain;
		s_Data->RenderCommand = RenderCommand::Create();

		CommandBufferCreateInfo commandBufferCreateInfo{};
		commandBufferCreateInfo.Level = Enumerations::CommandBufferLevel::Primary;
		commandBufferCreateInfo.Usage = Enumerations::CommandBufferUsageFlags::ONE_TIME_SUBMIT_BIT;
		commandBufferCreateInfo.TargetSwapChain = true;
		s_Data->DrawCommandBuffers = CommandBuffer::Create(commandBufferCreateInfo, s_Data->InitializeInfo.FramesInFlight);
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

		s_Data->DrawCommandBuffers->Reset(s_Data->CurrentFrameIndex);
		s_Data->DrawCommandBuffers->Begin(s_Data->CurrentFrameIndex);
	}

	void Renderer::EndFrame()
	{
		s_Data->DrawCommandBuffers->End(s_Data->CurrentFrameIndex);
		s_Data->DrawCommandBuffers->Submit(s_Data->CurrentFrameIndex);
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

	CommandBuffer* Renderer::GetDrawCommandBuffer()
	{
		return s_Data->DrawCommandBuffers;
	}

}