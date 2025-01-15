#include "ATPCH.h"
#include "TestRenderer.h"

#include "Renderer.h"

namespace Atom
{

	TestRenderer* TestRenderer::Create()
	{
		return new TestRenderer();
	}

	TestRenderer::TestRenderer()
		: m_RenderCommand(RenderCommand::Create()), m_CommandBuffer(CommandBuffer::Create())
	{
	}

	TestRenderer::~TestRenderer()
	{
		delete m_RenderCommand;
		m_RenderCommand = nullptr;

		delete m_CommandBuffer;
		m_CommandBuffer = nullptr;
	}

	void TestRenderer::BeginScene() const
	{
		m_RenderCommand->ResetCommandBuffer(m_CommandBuffer, Renderer::GetCurrentFrameIndex());
		m_RenderCommand->BeginCommandBuffer(m_CommandBuffer, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::EndScene() const
	{
		m_RenderCommand->EndCommandBuffer(m_CommandBuffer, Renderer::GetCurrentFrameIndex());
		m_RenderCommand->SubmitCommandBuffer(m_CommandBuffer, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::BeginRenderPass(RenderPass* renderPass) const
	{
		m_RenderCommand->BeginRenderPass(m_CommandBuffer, renderPass, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::EndRenderPass() const
	{
		m_RenderCommand->EndRenderPass(m_CommandBuffer, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::DrawStaticTriangle(Pipeline* pipeline)
	{
		m_RenderCommand->RenderStaticPipeline(m_CommandBuffer, pipeline, 3, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::DrawVertices(Pipeline* pipeline, VertexBuffer* vertexBuffer, uint32_t vertexCount)
	{
		m_RenderCommand->DrawVertices(m_CommandBuffer, pipeline, vertexBuffer, vertexCount, Renderer::GetCurrentFrameIndex());
	}

	void TestRenderer::DrawIndexed(Pipeline* pipeline, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint32_t indexCount)
	{
		m_RenderCommand->DrawIndexed(m_CommandBuffer, pipeline, vertexBuffer, indexBuffer, indexCount, Renderer::GetCurrentFrameIndex());
	}

}