#include "ATPCH.h"
#include "Atom/Graphics/Renderer2D.h"

#include "Atom/Core/Application.h"

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

		uint32_t maxIndices = m_Capabilities.MaxIndices;
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

		m_LineShader = Shader::CreateFromFile("Assets/Shaders/Renderer2D_Line.shader");
		m_QuadShader = Shader::CreateFromFile("Assets/Shaders/Renderer2D_Quad.shader");

		m_QuadTransformDataBase = new Renderer2D::QuadTransformData[m_Capabilities.MaxQuads];
		m_StorageBuffer = StorageBuffer::Create(sizeof(Renderer2D::QuadTransformData) * m_Capabilities.MaxQuads);

		m_QuadPipeline = CreateQuadPipeline(m_QuadShader);
		m_LinePipeline = CreateLinePipeline(m_LineShader);
	}

	Renderer2D::~Renderer2D()
	{
		DestroyLinePipeline();
		DestroyQuadPipeline();

		delete m_LineShader;
		m_LineShader = nullptr;

		delete m_QuadShader;
		m_QuadShader = nullptr;

		delete m_StorageBuffer;
		m_StorageBuffer = nullptr;

		delete m_QuadIndexBuffer;
		m_QuadIndexBuffer = nullptr;

		delete m_CameraUniformBuffer;
		m_CameraUniformBuffer = nullptr;

		delete m_CommandBuffer;
		m_CommandBuffer = nullptr;
	}

	void Renderer2D::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& e)
		{
			return OnWindowResizeEvent(e);
		});
	}

	void Renderer2D::Begin(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
	{
		m_CameraUBO.Projection = projectionMatrix;
		m_CameraUBO.View = viewMatrix;
		m_CameraUniformBuffer->Upload((void*)&m_CameraUBO, sizeof(m_CameraUBO), Renderer::GetCurrentFrameIndex());

		StartBatch();
	}

	void Renderer2D::End()
	{
		Flush();

		/*m_CommandBuffer->Reset(Renderer::GetCurrentFrameIndex());
		m_CommandBuffer->Begin(Renderer::GetCurrentFrameIndex());*/

		// TODO: RENDER!!!

		//m_CommandBuffer->End(Renderer::GetCurrentFrameIndex());
		// TODO: CMDBUFFER->EXECUTE
	}

	void Renderer2D::SubmitQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		SubmitQuad({ position.x, position.y, 0.0f }, { size.x, size.y, 1.0f }, color);
	}

	void Renderer2D::SubmitQuad(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color)
	{
		if (m_QuadPipeline.IndexCount >= m_Capabilities.MaxIndices)
		{
			NextBatch();
		}

		for (size_t i = 0; i < 4; i++)
		{
			m_QuadPipeline.VertexBufferPtr->VertexPosition = m_QuadVertexPositions[i];
			m_QuadPipeline.VertexBufferPtr->Color = color;
			m_QuadPipeline.VertexBufferPtr->QuadIndex = m_QuadTransformDataCount;
			m_QuadPipeline.VertexBufferPtr++;
		}

		m_QuadTransformDataPtr->Position = position;
		m_QuadTransformDataPtr->Scale = size;
		m_QuadTransformDataPtr++;

		m_QuadPipeline.IndexCount += 6;
		m_QuadTransformDataCount++;
	}

	void Renderer2D::SubmitLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness)
	{
		SubmitLine({ start.x, start.y, 0.0f }, { end.x, end.y, 0.0f }, color, thickness);
	}

	void Renderer2D::SubmitLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float thickness)
	{
		if (m_LinePipeline.IndexCount >= m_Capabilities.MaxIndices)
		{
#if 1
			NextBatch();
#else
			// TODO: resize buffers (VertexBuffer, VertexBufferBase, probably IndexBuffer aswell)!
			AT_CORE_ASSERT(false, "Resizing is needed!!");
			return;
#endif
		}

		glm::vec2 normal = glm::normalize(glm::vec2(end.y - start.y, -(end.x - start.x))) * thickness;
		glm::vec3 vertex1 = glm::vec3(start.x + normal.x, start.y + normal.y, 0.0f);
		glm::vec3 vertex2 = glm::vec3(end.x + normal.x, end.y + normal.y, 0.0f);
		glm::vec3 vertex3 = glm::vec3(end.x - normal.x, end.y - normal.y, 0.0f);
		glm::vec3 vertex4 = glm::vec3(start.x - normal.x, start.y - normal.y, 0.0f);

		for (size_t i = 0; i < 4; i++)
		{
			glm::vec3 vertex;
			if (i == 0) vertex = vertex1;
			if (i == 1) vertex = vertex2;
			if (i == 2) vertex = vertex3;
			if (i == 3) vertex = vertex4;

			m_LinePipeline.VertexBufferPtr->Position = vertex;
			m_LinePipeline.VertexBufferPtr->Color = color;
			m_LinePipeline.VertexBufferPtr++;
		}


		m_LinePipeline.IndexCount += 6;
	}

	bool Renderer2D::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		m_QuadPipeline.RenderPass->Resize(event.GetWidth(), event.GetHeight());
		m_LinePipeline.RenderPass->Resize(event.GetWidth(), event.GetHeight());

		return false;
	}

	void Renderer2D::StartBatch()
	{
		m_QuadPipeline.IndexCount = 0;
		m_QuadPipeline.VertexBufferPtr = m_QuadPipeline.VertexBufferBase;

		m_QuadTransformDataCount = 0;
		m_QuadTransformDataPtr = m_QuadTransformDataBase;

		m_LinePipeline.IndexCount = 0;
		m_LinePipeline.VertexBufferPtr = m_LinePipeline.VertexBufferBase;
	}

	template<typename T>
	static void DrawPipeline(RenderCommand* renderCommand, CommandBuffer* commandBuffer, Renderer2D::Pipeline2D<T>& pipeline, IndexBuffer* indexBuffer, uint32_t frameIndex)
	{
		if (pipeline.IndexCount == 0)
		{
			return;
		}

		renderCommand->BeginRenderPass(commandBuffer, pipeline.RenderPass, frameIndex);

		if (pipeline.IndexCount != pipeline.PreviousIndexCount)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)pipeline.VertexBufferPtr - (uint8_t*)pipeline.VertexBufferBase);
			pipeline.VertexBuffer->Upload(dataSize, pipeline.VertexBufferBase);
		}

		renderCommand->DrawIndexed(commandBuffer, pipeline.Pipeline, pipeline.VertexBuffer, indexBuffer, pipeline.IndexCount, frameIndex);

		renderCommand->EndRenderPass(commandBuffer, frameIndex);

		pipeline.PreviousIndexCount = pipeline.IndexCount;
	}

	void Renderer2D::Flush()
	{
		CommandBuffer* drawCommandBuffer = Renderer::GetDrawCommandBuffer();
		RenderCommand* renderCommand = Renderer::GetRenderCommand();

		{	// QUAD
			if (m_QuadPipeline.IndexCount)
			{
				if (m_QuadTransformDataCount)
				{
					uint32_t size = (uint32_t)((uint8_t*)m_QuadTransformDataPtr - (uint8_t*)m_QuadTransformDataBase);
					m_StorageBuffer->Upload(size, m_QuadTransformDataBase, Renderer::GetCurrentFrameIndex());
				}

				renderCommand->BeginRenderPass(drawCommandBuffer, m_QuadPipeline.RenderPass, Renderer::GetCurrentFrameIndex());

				if (m_QuadPipeline.IndexCount != m_QuadPipeline.PreviousIndexCount)
				{
					uint32_t dataSize = (uint32_t)((uint8_t*)m_QuadPipeline.VertexBufferPtr - (uint8_t*)m_QuadPipeline.VertexBufferBase);
					m_QuadPipeline.VertexBuffer->Upload(dataSize, m_QuadPipeline.VertexBufferBase);
				}

				renderCommand->DrawIndexed(drawCommandBuffer, m_QuadPipeline.Pipeline, m_QuadPipeline.VertexBuffer, m_QuadIndexBuffer, m_QuadPipeline.IndexCount, Renderer::GetCurrentFrameIndex());

				renderCommand->EndRenderPass(drawCommandBuffer, Renderer::GetCurrentFrameIndex());

				m_QuadPipeline.PreviousIndexCount = m_QuadPipeline.IndexCount;
			}
		}

		DrawPipeline(renderCommand, drawCommandBuffer, m_LinePipeline, m_QuadIndexBuffer, Renderer::GetCurrentFrameIndex());
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	Renderer2D::Pipeline2D<Renderer2D::QuadVertex> Renderer2D::CreateQuadPipeline(Shader* shader)
	{
		glm::vec2 windowSize = { Application::Get().GetWindow()->GetWidth(), Application::Get().GetWindow()->GetHeight() };

		RenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.ImageFormat = Enumerations::ImageFormat::B8G8R8A8_UNORM;
		renderPassCreateInfo.LoadOperation = Enumerations::RenderPassAttachmentLoadOperation::Load;
		renderPassCreateInfo.RenderArea = windowSize;
		renderPassCreateInfo.TargetSwapChain = true;
		RenderPass* renderPass = RenderPass::Create(renderPassCreateInfo);

		PipelineOptions pipelineOptions{};
		pipelineOptions.Layout = {
			 { Enumerations::ShaderDataType::Float4, "inVertexPosition" },
			 { Enumerations::ShaderDataType::Float4, "inColor" },
			 { Enumerations::ShaderDataType::Int, "inQuadIndex" }
		};
		pipelineOptions.Shader = shader;
		pipelineOptions.RenderPass = renderPass;
		pipelineOptions.UniformBuffer = m_CameraUniformBuffer;
		pipelineOptions.StorageBuffer = m_StorageBuffer;

		VertexBufferCreateInfo vertexBufferCreateInfo{};
		vertexBufferCreateInfo.Usage = Enumerations::BufferUsageFlags::VertexBuffer;
		vertexBufferCreateInfo.Size = sizeof(Renderer2D::QuadVertex) * m_Capabilities.MaxVertices;

		Renderer2D::Pipeline2D<Renderer2D::QuadVertex> pipeline{};
		pipeline.Shader = shader;
		pipeline.Pipeline = Pipeline::Create(pipelineOptions);
		pipeline.RenderPass = renderPass;
		pipeline.VertexBuffer = VertexBuffer::Create(vertexBufferCreateInfo);
		pipeline.VertexBufferBase = new Renderer2D::QuadVertex[m_Capabilities.MaxVertices];
		return pipeline;
	}

	void Renderer2D::DestroyQuadPipeline()
	{
		delete m_QuadPipeline.Pipeline;
		m_QuadPipeline.Pipeline = nullptr;

		delete m_QuadPipeline.RenderPass;
		m_QuadPipeline.RenderPass = nullptr;

		delete m_QuadPipeline.VertexBuffer;
		m_QuadPipeline.VertexBuffer = nullptr;

		delete m_QuadPipeline.VertexBufferBase;
		m_QuadPipeline.VertexBufferBase = nullptr;
		m_QuadPipeline.VertexBufferPtr = nullptr;
	}

	Renderer2D::Pipeline2D<Renderer2D::LineVertex> Renderer2D::CreateLinePipeline(Shader* shader)
	{
		glm::vec2 windowSize = { Application::Get().GetWindow()->GetWidth(), Application::Get().GetWindow()->GetHeight() };

		RenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.ImageFormat = Enumerations::ImageFormat::B8G8R8A8_UNORM;
		renderPassCreateInfo.LoadOperation = Enumerations::RenderPassAttachmentLoadOperation::Load;
		renderPassCreateInfo.RenderArea = windowSize;
		renderPassCreateInfo.TargetSwapChain = true;
		RenderPass* renderPass = RenderPass::Create(renderPassCreateInfo);

		PipelineOptions pipelineOptions{};
		pipelineOptions.Layout = {
			{ Enumerations::ShaderDataType::Float3, "inPosition" },
			{ Enumerations::ShaderDataType::Float4, "inColor" }
		};
		pipelineOptions.Shader = shader;
		pipelineOptions.RenderPass = renderPass;
		pipelineOptions.UniformBuffer = m_CameraUniformBuffer;

		VertexBufferCreateInfo vertexBufferCreateInfo{};
		vertexBufferCreateInfo.Usage = Enumerations::BufferUsageFlags::VertexBuffer;
		vertexBufferCreateInfo.Size = sizeof(Renderer2D::LineVertex) * m_Capabilities.MaxVertices;

		Renderer2D::Pipeline2D<Renderer2D::LineVertex> pipeline{};
		pipeline.Shader = shader;
		pipeline.Pipeline = Pipeline::Create(pipelineOptions);
		pipeline.RenderPass = renderPass;
		pipeline.VertexBuffer = VertexBuffer::Create(vertexBufferCreateInfo);
		pipeline.VertexBufferBase = new Renderer2D::LineVertex[m_Capabilities.MaxVertices];
		return pipeline;
	}

	void Renderer2D::DestroyLinePipeline()
	{
		delete m_LinePipeline.Pipeline;
		m_LinePipeline.Pipeline = nullptr;

		delete m_LinePipeline.RenderPass;
		m_LinePipeline.RenderPass = nullptr;

		delete m_LinePipeline.VertexBuffer;
		m_LinePipeline.VertexBuffer = nullptr;

		delete m_LinePipeline.VertexBufferBase;
		m_LinePipeline.VertexBufferBase = nullptr;
		m_LinePipeline.VertexBufferPtr = nullptr;
	}

}
