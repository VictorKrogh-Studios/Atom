#include "ATPCH.h"
#include "TestRenderer.h"

#include "Atom/Graphics/Renderer.h"
#include "Atom/Graphics/RenderCommand.h"

namespace Atom
{

	TestRenderer* TestRenderer::Create()
	{
		return new TestRenderer();
	}

	TestRenderer::TestRenderer()
		: m_RenderCommand(Renderer::GetRenderCommand())
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
		m_RenderCommand->BeginRenderPass(Renderer::GetDrawCommandBuffer(), renderPass, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::EndRenderPass() const
	{
		m_RenderCommand->EndRenderPass(Renderer::GetDrawCommandBuffer(), Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::DrawStaticTriangle(Pipeline* pipeline)
	{
		m_RenderCommand->RenderStaticPipeline(Renderer::GetDrawCommandBuffer(), pipeline, 3, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::DrawVertices(Pipeline* pipeline, VertexBuffer* vertexBuffer, uint32_t vertexCount)
	{
		m_RenderCommand->DrawVertices(Renderer::GetDrawCommandBuffer(), pipeline, vertexBuffer, vertexCount, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::DrawIndexed(Pipeline* pipeline, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint32_t indexCount)
	{
		m_RenderCommand->DrawIndexed(Renderer::GetDrawCommandBuffer(), pipeline, vertexBuffer, indexBuffer, indexCount, Renderer::GetCurrentFrameIndex());
	}

}