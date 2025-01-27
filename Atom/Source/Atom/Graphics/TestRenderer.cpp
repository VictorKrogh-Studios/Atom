#include "ATPCH.h"
#include "Atom/Graphics/TestRenderer.h"

#include "Atom/Graphics/Renderer.h"
#include "Atom/Graphics/RenderCommand.h"

namespace Atom
{

	TestRenderer* TestRenderer::Create()
	{
		return new TestRenderer();
	}

	TestRenderer::TestRenderer()
	{
	}

	TestRenderer::~TestRenderer()
	{
	}

	void TestRenderer::BeginScene() const
	{
	}

	void TestRenderer::EndScene() const
	{
	}

	void TestRenderer::BeginRenderPass(RenderPass* renderPass) const
	{
		Renderer::GetRenderCommand()->BeginRenderPass(Renderer::GetDrawCommandBuffer(), renderPass, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::EndRenderPass() const
	{
		Renderer::GetRenderCommand()->EndRenderPass(Renderer::GetDrawCommandBuffer(), Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::DrawStaticTriangle(Pipeline* pipeline)
	{
		Renderer::GetRenderCommand()->RenderStaticPipeline(Renderer::GetDrawCommandBuffer(), pipeline, 3, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::DrawVertices(Pipeline* pipeline, VertexBuffer* vertexBuffer, uint32_t vertexCount)
	{
		Renderer::GetRenderCommand()->DrawVertices(Renderer::GetDrawCommandBuffer(), pipeline, vertexBuffer, vertexCount, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::DrawIndexed(Pipeline* pipeline, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint32_t indexCount)
	{
		Renderer::GetRenderCommand()->DrawIndexed(Renderer::GetDrawCommandBuffer(), pipeline, vertexBuffer, indexBuffer, indexCount, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::DrawTexturedQuad(Pipeline* pipeline, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, Texture* texture)
	{
	}

}