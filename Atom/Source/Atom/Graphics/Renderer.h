#pragma once

#include "Atom/Graphics/Pipeline.h"

namespace Atom
{

	class Renderer
	{
	public:
		static void Initialize();
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();

		static void DrawStaticTriangle(Pipeline* pipeline, uint32_t vertexCount = 3);
	};

}
