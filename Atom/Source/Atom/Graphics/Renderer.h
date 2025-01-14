#pragma once

#include "Atom/Graphics/Pipeline.h"

namespace Atom
{

	struct RendererInitializeInfo
	{
		uint32_t FramesInFlight;
	};

	class Renderer
	{
	public:
		static void Initialize(const RendererInitializeInfo& initializeInfo);
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void BeginScene();
		static void EndScene();

		static void DrawStaticTriangle(Pipeline* pipeline, uint32_t vertexCount = 3);

		static uint32_t GetFramesInFlight();
	};

}
