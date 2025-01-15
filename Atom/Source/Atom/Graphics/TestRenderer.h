#pragma once

#include "Atom/Graphics/RenderCommand.h"
#include "Atom/Graphics/CommandBuffer.h"
#include "Atom/Graphics/Pipeline.h"

namespace Atom
{

	class TestRenderer
	{
	public:
		static TestRenderer* Create();
	public:
		TestRenderer();
		~TestRenderer();

		void BeginScene() const;
		void EndScene() const;

		void DrawStaticTriangle(Pipeline* pipeline);
	private:
		RenderCommand* m_RenderCommand = nullptr;
		CommandBuffer* m_CommandBuffer = nullptr;
	};

}
