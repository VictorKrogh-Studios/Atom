#include "ATPCH.h"
#include "Renderer2D.h"

#include "Atom/Graphics/Renderer.h"
#include "Atom/Graphics/RenderCommand.h"
#include "Atom/Graphics/Shader.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Atom
{

	Renderer2D::Renderer2D(const Renderer2DCapabilities& capabilities)
		: m_Capabilities(capabilities), m_CameraUBO({})
	{
		CommandBufferCreateInfo commandBufferCreateInfo{};
		commandBufferCreateInfo.Level = Enumerations::CommandBufferLevel::Secondary;
		commandBufferCreateInfo.Usage = Enumerations::CommandBufferUsageFlags::RENDER_PASS_CONTINUE_BIT;
		m_CommandBuffer = CommandBuffer::Create(commandBufferCreateInfo);

		RenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.ImageFormat = Enumerations::ImageFormat::B8G8R8A8_UNORM;
		renderPassCreateInfo.LoadOperation = Enumerations::RenderPassAttachmentLoadOperation::Load;
		renderPassCreateInfo.RenderArea = { 1600, 900 };
		renderPassCreateInfo.TargetSwapChain = true;
		m_RenderPass = RenderPass::Create(renderPassCreateInfo);

		uint32_t maxIndices = m_Capabilities.GetMaxIndices();
		uint32_t* quadIndices = new uint32_t[maxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < maxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		m_QuadIndexBuffer = IndexBuffer::Create(maxIndices, quadIndices);

		m_CameraUniformBuffer = UniformBuffer::Create(sizeof(m_CameraUBO));

		m_QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		m_QuadPipeline = CreateQuadPipeline();
	}

	Renderer2D::~Renderer2D()
	{
		DestroyQuadPipeline();

		delete m_QuadIndexBuffer;
		m_QuadIndexBuffer = nullptr;

		delete m_RenderPass;
		m_RenderPass = nullptr;

		delete m_CameraUniformBuffer;
		m_CameraUniformBuffer = nullptr;

		delete m_CommandBuffer;
		m_CommandBuffer = nullptr;
	}

	void Renderer2D::Begin(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
	{
		m_CameraUBO.Projection = projectionMatrix;
		m_CameraUBO.View = viewMatrix;
		m_CameraUniformBuffer->Upload((void*)&m_CameraUBO, sizeof(m_CameraUBO), Renderer::GetCurrentFrameIndex());

		m_QuadPipeline.IndexCount = 0;
		m_QuadPipeline.VertexBufferPtr = m_QuadPipeline.VertexBufferBase;
	}

	void Renderer2D::End()
	{
		CommandBuffer* drawCommandBuffer = Renderer::GetDrawCommandBuffer();
		RenderCommand* renderCommand = Renderer::GetRenderCommand();

		renderCommand->BeginRenderPass(drawCommandBuffer, m_RenderPass, Renderer::GetCurrentFrameIndex());

		if (m_QuadPipeline.IndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)m_QuadPipeline.VertexBufferPtr - (uint8_t*)m_QuadPipeline.VertexBufferBase);
			m_QuadPipeline.VertexBuffer->Upload(dataSize, m_QuadPipeline.VertexBufferBase);

			renderCommand->DrawIndexed(drawCommandBuffer, m_QuadPipeline.Pipeline, m_QuadPipeline.VertexBuffer, m_QuadIndexBuffer, m_QuadPipeline.IndexCount, Renderer::GetCurrentFrameIndex());

			// s_Renderer2DData.Stats.DrawCalls++;
		}

		renderCommand->EndRenderPass(drawCommandBuffer, Renderer::GetCurrentFrameIndex());

		/*m_CommandBuffer->Reset(Renderer::GetCurrentFrameIndex());
		m_CommandBuffer->Begin(Renderer::GetCurrentFrameIndex());*/

		// TODO: RENDER!!!

		//m_CommandBuffer->End(Renderer::GetCurrentFrameIndex());
		// TODO: CMDBUFFER->EXECUTE
	}

	void Renderer2D::SubmitQuad(const glm::vec2& position, const glm::vec2 size, const glm::vec4& color)
	{
		SubmitQuad({ position.x, position.y, 0.0f }, size, color);
	}

	void Renderer2D::SubmitQuad(const glm::vec3& position, const glm::vec2 size, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

		SubmitQuad(transform, color);
	}

	void Renderer2D::SubmitQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		for (size_t i = 0; i < 4; i++)
		{
			m_QuadPipeline.VertexBufferPtr->Position = transform * m_QuadVertexPositions[i];
			m_QuadPipeline.VertexBufferPtr->Color = color;
			m_QuadPipeline.VertexBufferPtr++;
		}

		m_QuadPipeline.IndexCount += 6;
	}

	Renderer2D::Pipeline2D<Renderer2D::QuadVertex> Renderer2D::CreateQuadPipeline()
	{
		Shader* shader = Shader::CreateFromFile("Assets/Shaders/Renderer2D_Quad_vert.spv", "Assets/Shaders/Renderer2D_Quad_frag.spv");

		PipelineOptions pipelineOptions{};
		pipelineOptions.Layout = {
			{ Enumerations::ShaderDataType::Float3, "inPosition" },
			{ Enumerations::ShaderDataType::Float4, "inColor" }
		};
		pipelineOptions.Shader = shader;
		pipelineOptions.RenderPass = m_RenderPass;
		pipelineOptions.UniformBuffer = m_CameraUniformBuffer;

		VertexBufferCreateInfo vertexBufferCreateInfo{};
		vertexBufferCreateInfo.Usage = Enumerations::BufferUsageFlags::VertexBuffer;
		vertexBufferCreateInfo.Size = m_Capabilities.GetMaxVertices();

		Renderer2D::Pipeline2D<Renderer2D::QuadVertex> pipeline{};
		pipeline.Shader = shader;
		pipeline.Pipeline = Pipeline::Create(pipelineOptions);
		pipeline.VertexBuffer = VertexBuffer::Create(vertexBufferCreateInfo);
		pipeline.VertexBufferBase = new Renderer2D::QuadVertex[m_Capabilities.GetMaxVertices()];
		return pipeline;
	}

	void Renderer2D::DestroyQuadPipeline()
	{
		delete m_QuadPipeline.Pipeline;
		m_QuadPipeline.Pipeline = nullptr;

		delete m_QuadPipeline.Shader;
		m_QuadPipeline.Shader = nullptr;

		delete m_QuadPipeline.VertexBuffer;
		m_QuadPipeline.VertexBuffer = nullptr;

		delete m_QuadPipeline.VertexBufferBase;
		m_QuadPipeline.VertexBufferBase = nullptr;
	}

}
