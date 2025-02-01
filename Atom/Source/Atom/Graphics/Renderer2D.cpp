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
		: m_Capabilities(capabilities), m_Statistics({}), m_CameraUBO({})
	{
		Renderer::GetShaderLibrary()->Load("Renderer2D_Quad", "Assets/Shaders/Renderer2D_Quad.shader");
		Renderer::GetShaderLibrary()->Load("Renderer2D_Line", "Assets/Shaders/Renderer2D_Line.shader");

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

		m_QuadTransformDatas.resize(m_Capabilities.MaxQuads);
		m_QuadTransformDataStorageBuffer = StorageBuffer::Create(sizeof(Renderer2D::QuadTransformData) * m_Capabilities.MaxQuads);

		m_QuadVertexBuffers.resize(1);
		m_QuadVertexBufferBases.resize(1);
		m_QuadVertexBufferPtr.resize(1);

		uint32_t framesInFlight = Renderer::GetFramesInFlight();

		m_QuadPipeline = CreateQuadPipeline(framesInFlight);
		m_LinePipeline = CreateLinePipeline();
	}

	Renderer2D::~Renderer2D()
	{
		DestroyLinePipeline();
		DestroyQuadPipeline();

		delete m_QuadTransformDataStorageBuffer;
		m_QuadTransformDataStorageBuffer = nullptr;

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
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

		memset(&m_Statistics, 0, sizeof(Renderer2DStatistics));

		m_CameraUBO.Projection = projectionMatrix;
		m_CameraUBO.View = viewMatrix;
		m_CameraUniformBuffer->Upload((void*)&m_CameraUBO, sizeof(m_CameraUBO), frameIndex);

		m_QuadIndexCount = 0;
		for (uint32_t i = 0; i < m_QuadVertexBufferPtr.size(); i++)
		{
			m_QuadVertexBufferPtr[i] = m_QuadVertexBufferBases[i][frameIndex];
		}

		StartBatch();
	}

	void Renderer2D::End()
	{
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

		if (m_QuadTransformDataCount)
		{
			uint32_t size = sizeof(Renderer2D::QuadTransformData) * m_QuadTransformDataCount;
			m_QuadTransformDataStorageBuffer->Resize(size, frameIndex);
			m_QuadTransformDataStorageBuffer->Upload(size, m_QuadTransformDatas.data(), frameIndex);
			m_QuadPipeline.Pipeline->Set(1, m_QuadTransformDataStorageBuffer);
		}

		Flush();
	}

	void Renderer2D::SubmitQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		SubmitQuad({ position.x, position.y, 0.0f }, { size.x, size.y, 1.0f }, color);
	}

	void Renderer2D::SubmitQuad(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color)
	{
		auto& quadVertexBufferPtr = GetWriteableQuadBuffer();
		auto& quadTransformDataPtr = GetQuadTransformDataPtr();

		for (size_t i = 0; i < 4; i++)
		{
			quadVertexBufferPtr->VertexPosition = m_QuadVertexPositions[i];
			quadVertexBufferPtr->Color = color;
			quadVertexBufferPtr->QuadIndex = m_QuadTransformDataCount;
			quadVertexBufferPtr++;
		}
		 
		quadTransformDataPtr.Position = position;
		quadTransformDataPtr.Scale = size;

		m_QuadIndexCount += 6;

		m_QuadPipeline.IndexCount += 6;
		m_QuadTransformDataCount++;

		m_Statistics.QuadCount++;
	}

	void Renderer2D::SubmitLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness)
	{
		SubmitLine({ start.x, start.y, 0.0f }, { end.x, end.y, 0.0f }, color, thickness);
	}

	void Renderer2D::SubmitLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float thickness)
	{
		if (m_LinePipeline.IndexCount >= m_Capabilities.MaxIndices)
		{
#if 0
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

		m_Statistics.LineCount++;
	}

	bool Renderer2D::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		m_QuadPipeline.RenderPass->Resize(event.GetWidth(), event.GetHeight());
		m_LinePipeline.RenderPass->Resize(event.GetWidth(), event.GetHeight());

		return false;
	}

	void Renderer2D::AddQuadVertexBuffer()
	{
		uint32_t framesInFlight = Renderer::GetFramesInFlight();

		std::vector<VertexBuffer*>& newVertexBuffers = m_QuadVertexBuffers.emplace_back();
		std::vector<QuadVertex*>& newVertexBufferBases = m_QuadVertexBufferBases.emplace_back();

		newVertexBuffers.resize(framesInFlight);
		newVertexBufferBases.resize(framesInFlight);
		for (uint32_t i = 0; i < framesInFlight; i++)
		{
			VertexBufferCreateInfo vertexBufferCreateInfo{};
			vertexBufferCreateInfo.Usage = Enumerations::BufferUsageFlags::VertexBuffer;
			vertexBufferCreateInfo.Size = sizeof(Renderer2D::QuadVertex) * m_Capabilities.MaxVertices;
			newVertexBuffers[i] = VertexBuffer::Create(vertexBufferCreateInfo);
			newVertexBufferBases[i] = new QuadVertex[m_Capabilities.MaxVertices];
		}
	}

	Renderer2D::QuadVertex*& Renderer2D::GetWriteableQuadBuffer()
	{
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

		m_QuadBufferWriteIndex = m_QuadIndexCount / m_Capabilities.MaxIndices;
		if (m_QuadBufferWriteIndex >= m_QuadVertexBufferBases.size())
		{
			AddQuadVertexBuffer();
			m_QuadVertexBufferPtr.emplace_back();
			m_QuadVertexBufferPtr[m_QuadBufferWriteIndex] = m_QuadVertexBufferBases[m_QuadBufferWriteIndex][frameIndex];
		}

		return m_QuadVertexBufferPtr[m_QuadBufferWriteIndex];
	}

	Renderer2D::QuadTransformData& Atom::Renderer2D::GetQuadTransformDataPtr()
	{
		if (m_QuadTransformDataCount >= m_QuadTransformDatas.size())
		{
			m_QuadTransformDatas.emplace_back(Renderer2D::QuadTransformData{});
		}

		return m_QuadTransformDatas.at(m_QuadTransformDataCount);
	}

	void Renderer2D::StartBatch()
	{
		m_QuadPipeline.IndexCount = 0;
		m_QuadPipeline.VertexBufferPtr = m_QuadPipeline.VertexBufferBase;

		m_QuadTransformDataCount = 0;

		m_LinePipeline.IndexCount = 0;
		m_LinePipeline.VertexBufferPtr = m_LinePipeline.VertexBufferBase;
	}

	template<typename T>
	static void DrawPipeline(RenderCommand* renderCommand, CommandBuffer* commandBuffer, Renderer2D::Pipeline2D<T>& pipeline, IndexBuffer* indexBuffer, uint32_t frameIndex, Renderer2DStatistics& stats)
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

		stats.DrawCalls++;
	}

	void Renderer2D::Flush()
	{
		CommandBuffer* drawCommandBuffer = Renderer::GetDrawCommandBuffer();
		RenderCommand* renderCommand = Renderer::GetRenderCommand();
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

		CommandBuffer* drawCommandBuffer = Renderer::GetDrawCommandBuffer();
		RenderCommand* renderCommand = Renderer::GetRenderCommand();

		renderCommand->BeginRenderPass(drawCommandBuffer, m_QuadPipeline.RenderPass, frameIndex);

		m_CommandBuffer->Begin(m_QuadPipeline.RenderPass, frameIndex);

		renderCommand->SetViewport(m_CommandBuffer, m_QuadPipeline.RenderPass, frameIndex);
		for (uint32_t i = 0; i <= m_QuadBufferWriteIndex; i++)
		{
			uint32_t dataSize = (uint32_t)((uint8_t*)m_QuadVertexBufferPtr[i] - (uint8_t*)m_QuadVertexBufferBases[i][frameIndex]);
			if (dataSize)
			{
				m_QuadVertexBuffers[i][frameIndex]->Upload(dataSize, m_QuadVertexBufferBases[i][frameIndex]);

				uint32_t indexCount = i == m_QuadBufferWriteIndex ? m_QuadIndexCount - (m_Capabilities.MaxIndices * i) : m_Capabilities.MaxIndices;

				renderCommand->DrawIndexed(m_CommandBuffer, m_QuadPipeline.Pipeline, m_QuadVertexBuffers[i][frameIndex], m_QuadIndexBuffer, indexCount, frameIndex);

				m_Statistics.DrawCalls++;
			}
		}

		m_CommandBuffer->End(frameIndex);
		m_CommandBuffer->Execute(drawCommandBuffer, frameIndex);

		renderCommand->EndRenderPass(drawCommandBuffer, frameIndex);

		DrawPipeline(renderCommand, drawCommandBuffer, m_LinePipeline, m_QuadIndexBuffer, Renderer::GetCurrentFrameIndex(), m_Statistics);
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

	Renderer2D::Pipeline2D<Renderer2D::QuadVertex> Renderer2D::CreateQuadPipeline(uint32_t framesInFlight)
	{
		glm::vec2 windowSize = { Application::Get().GetWindow()->GetWidth(), Application::Get().GetWindow()->GetHeight() };
		Shader* shader = Renderer::GetShaderLibrary()->Get("Renderer2D_Quad");

		RenderPassCreateInfo renderPassCreateInfo{};
		renderPassCreateInfo.ImageFormat = Application::Get().GetWindow()->GetImageFormat();
		renderPassCreateInfo.LoadOperation = Enumerations::RenderPassAttachmentLoadOperation::Load;
		renderPassCreateInfo.RenderArea = windowSize;
		renderPassCreateInfo.TargetSwapChain = true;
		renderPassCreateInfo.ImplicitSetViewport = false;
		renderPassCreateInfo.SubpassContents = Enumerations::RenderPassSubpassContents::SecondaryCommandBuffer;
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
		pipelineOptions.StorageBuffer = m_QuadTransformDataStorageBuffer;

		VertexBufferCreateInfo vertexBufferCreateInfo{};
		vertexBufferCreateInfo.Usage = Enumerations::BufferUsageFlags::VertexBuffer;
		vertexBufferCreateInfo.Size = sizeof(Renderer2D::QuadVertex) * m_Capabilities.MaxVertices;

		m_QuadVertexBuffers[0].resize(framesInFlight);
		m_QuadVertexBufferBases[0].resize(framesInFlight);
		for (uint32_t i = 0; i < framesInFlight; i++)
		{
			VertexBufferCreateInfo vertexBufferCreateInfo{};
			vertexBufferCreateInfo.Usage = Enumerations::BufferUsageFlags::VertexBuffer;
			vertexBufferCreateInfo.Size = sizeof(Renderer2D::QuadVertex) * m_Capabilities.MaxVertices;
			m_QuadVertexBuffers[0][i] = VertexBuffer::Create(vertexBufferCreateInfo);
			m_QuadVertexBufferBases[0][i] = new QuadVertex[m_Capabilities.MaxVertices];
		}

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
		for (auto& buffers : m_QuadVertexBufferBases)
		{
			for (auto buffer : buffers)
			{
				delete[] buffer;
			}
		}

		for (auto& buffers : m_QuadVertexBuffers)
		{
			for (auto buffer : buffers)
			{
				delete buffer;
			}
		}

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

	Renderer2D::Pipeline2D<Renderer2D::LineVertex> Renderer2D::CreateLinePipeline()
	{
		glm::vec2 windowSize = { Application::Get().GetWindow()->GetWidth(), Application::Get().GetWindow()->GetHeight() };
		Shader* shader = Renderer::GetShaderLibrary()->Get("Renderer2D_Line");

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
