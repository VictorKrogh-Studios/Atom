#pragma once

namespace Atom
{

	class SwapChain;

	struct RendererInitializeInfo
	{
		SwapChain* SwapChain;
		uint32_t FramesInFlight;
	};

	class Renderer
	{
	public:
		static void Initialize(const RendererInitializeInfo& initializeInfo);
		static void Shutdown();

		static void BeginFrame();		// This should only be called from the engine!
		static void EndFrame();			// This should only be called from the engine!
		static void PresentAndWait();	// This should only be called from the engine!

		static uint32_t GetCurrentFrameIndex();
		static uint32_t GetFramesInFlight();
	};

}
