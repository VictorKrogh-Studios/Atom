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

		m_CameraUniformBuffer = UniformBuffer::Create(sizeof(m_CameraUBO));

		uint32_t data = 0xFFFFFFFF;
		m_WhiteTexture = Texture::Create(1, 1, &data);
		m_TextureSlots[0] = m_WhiteTexture;

		m_QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[1] = { 0.5f, -0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[2] = { 0.5f,  0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };

		m_QuadVertexTexCoords[0] = { 0.0f, 0.0f };
		m_QuadVertexTexCoords[1] = { 1.0f, 0.0f };
		m_QuadVertexTexCoords[2] = { 1.0f, 1.0f };
		m_QuadVertexTexCoords[3] = { 0.0f, 1.0f };

		uint32_t framesInFlight = Renderer::GetFramesInFlight();

		glm::vec2 windowSize = { Application::Get().GetWindow()->GetWidth(), Application::Get().GetWindow()->GetHeight() };

		uint32_t maxIndices = m_Capabilities.MaxQuadIndices;
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

		{	// Setup quad pipeline
			m_QuadTransformDatas.resize(m_Capabilities.MaxQuads);
			m_QuadTransformDataStorageBuffer = StorageBuffer::Create(sizeof(Renderer2D::QuadTransformData) * m_Capabilities.MaxQuads);

			RenderPassCreateInfo renderPassCreateInfo{};
			renderPassCreateInfo.ImageFormat = Application::Get().GetWindow()->GetImageFormat();
			renderPassCreateInfo.LoadOperation = Enumerations::RenderPassAttachmentLoadOperation::Load;
			renderPassCreateInfo.RenderArea = windowSize;
			renderPassCreateInfo.TargetSwapChain = true;
			renderPassCreateInfo.ImplicitSetViewport = false;
			renderPassCreateInfo.SubpassContents = Enumerations::RenderPassSubpassContents::SecondaryCommandBuffer;
			m_QuadRenderPass = RenderPass::Create(renderPassCreateInfo);

			PipelineOptions pipelineOptions{};
			pipelineOptions.Layout = {
				 { Enumerations::ShaderDataType::Float4, "inVertexPosition" },
				 { Enumerations::ShaderDataType::Float4, "inColor" },
				 { Enumerations::ShaderDataType::Float2, "inTexCoord" },
				 { Enumerations::ShaderDataType::Int, "inTextureIndex" },
				 { Enumerations::ShaderDataType::Int, "inQuadIndex" }
			};
			pipelineOptions.Shader = Renderer::GetShaderLibrary()->Get("Renderer2D_Quad");
			pipelineOptions.RenderPass = m_QuadRenderPass;
			pipelineOptions.UniformBuffer = m_CameraUniformBuffer;
			pipelineOptions.StorageBuffer = m_QuadTransformDataStorageBuffer;
			m_QuadPipeline = Pipeline::Create(pipelineOptions);

			m_QuadIndexBuffer = IndexBuffer::Create(maxIndices, quadIndices);

			VertexBufferCreateInfo vertexBufferCreateInfo{};
			vertexBufferCreateInfo.Usage = Enumerations::BufferUsageFlags::VertexBuffer;
			vertexBufferCreateInfo.Size = sizeof(Renderer2D::QuadVertex) * m_Capabilities.MaxQuadVertices;

			m_QuadVertexBuffers.resize(1);
			m_QuadVertexBufferBases.resize(1);
			m_QuadVertexBufferPtr.resize(1);

			m_QuadVertexBuffers[0].resize(framesInFlight);
			m_QuadVertexBufferBases[0].resize(framesInFlight);
			for (uint32_t i = 0; i < framesInFlight; i++)
			{
				m_QuadVertexBuffers[0][i] = VertexBuffer::Create(vertexBufferCreateInfo);
				m_QuadVertexBufferBases[0][i] = new QuadVertex[m_Capabilities.MaxQuadVertices];
			}
		}

		{	// Setup line pipeline
			RenderPassCreateInfo renderPassCreateInfo{};
			renderPassCreateInfo.ImageFormat = Application::Get().GetWindow()->GetImageFormat();
			renderPassCreateInfo.LoadOperation = Enumerations::RenderPassAttachmentLoadOperation::Load;
			renderPassCreateInfo.RenderArea = windowSize;
			renderPassCreateInfo.TargetSwapChain = true;
			renderPassCreateInfo.ImplicitSetViewport = false;
			renderPassCreateInfo.SubpassContents = Enumerations::RenderPassSubpassContents::SecondaryCommandBuffer;
			m_LineRenderPass = RenderPass::Create(renderPassCreateInfo);

			PipelineOptions pipelineOptions{};
			pipelineOptions.Layout = {
				{ Enumerations::ShaderDataType::Float3, "inPosition" },
				{ Enumerations::ShaderDataType::Float4, "inColor" }
			};
			pipelineOptions.Shader = Renderer::GetShaderLibrary()->Get("Renderer2D_Line");
			pipelineOptions.RenderPass = m_LineRenderPass;
			pipelineOptions.UniformBuffer = m_CameraUniformBuffer;
			m_LinePipeline = Pipeline::Create(pipelineOptions);

			m_LineIndexBuffer = IndexBuffer::Create(maxIndices, quadIndices);

			VertexBufferCreateInfo vertexBufferCreateInfo{};
			vertexBufferCreateInfo.Usage = Enumerations::BufferUsageFlags::VertexBuffer;
			vertexBufferCreateInfo.Size = sizeof(Renderer2D::LineVertex) * m_Capabilities.MaxQuadVertices;

			m_LineVertexBuffers.resize(1);
			m_LineVertexBufferBases.resize(1);
			m_LineVertexBufferPtr.resize(1);

			m_LineVertexBuffers[0].resize(framesInFlight);
			m_LineVertexBufferBases[0].resize(framesInFlight);
			for (uint32_t i = 0; i < framesInFlight; i++)
			{
				m_LineVertexBuffers[0][i] = VertexBuffer::Create(vertexBufferCreateInfo);
				m_LineVertexBufferBases[0][i] = new LineVertex[m_Capabilities.MaxQuadVertices];
			}
		}

		delete[] quadIndices;
	}

	Renderer2D::~Renderer2D()
	{
		{	// Destroy quad pipeline
			for (auto& buffers : m_QuadVertexBufferBases)
			{
				for (auto buffer : buffers)
				{
					delete[] buffer;
				}
			}
			m_QuadVertexBufferBases.clear();

			for (auto& buffers : m_QuadVertexBuffers)
			{
				for (auto buffer : buffers)
				{
					delete buffer;
				}
			}
			m_QuadVertexBuffers.clear();

			delete m_QuadPipeline;
			m_QuadPipeline = nullptr;

			delete m_QuadRenderPass;
			m_QuadRenderPass = nullptr;

			delete m_QuadTransformDataStorageBuffer;
			m_QuadTransformDataStorageBuffer = nullptr;

			delete m_QuadIndexBuffer;
			m_QuadIndexBuffer = nullptr;
		}

		{	// Destroy line pipeline
			for (auto& buffers : m_LineVertexBufferBases)
			{
				for (auto buffer : buffers)
				{
					delete[] buffer;
				}
			}
			m_LineVertexBufferBases.clear();

			for (auto& buffers : m_LineVertexBuffers)
			{
				for (auto buffer : buffers)
				{
					delete buffer;
				}
			}
			m_LineVertexBuffers.clear();

			delete m_LinePipeline;
			m_LinePipeline = nullptr;

			delete m_LineRenderPass;
			m_LineRenderPass = nullptr;

			delete m_LineIndexBuffer;
			m_LineIndexBuffer = nullptr;
		}

		delete m_WhiteTexture;
		m_WhiteTexture = nullptr;

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
			m_QuadPipeline->Set(1, m_QuadTransformDataStorageBuffer);
		}

		for (uint32_t i = 0; i < m_TextureSlots.size(); i++)
		{
			if (m_TextureSlots[i])
			{
				m_QuadPipeline->Set(2, m_TextureSlots[i], i);
			}
			else
			{
				m_QuadPipeline->Set(2, m_WhiteTexture, i);
			}
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
			quadVertexBufferPtr->TexCoord = m_QuadVertexTexCoords[i];
			quadVertexBufferPtr->TextureIndex = 0;
			quadVertexBufferPtr->QuadIndex = m_QuadTransformDataCount;
			quadVertexBufferPtr++;
		}

		quadTransformDataPtr.Position = position;
		quadTransformDataPtr.Scale = size;

		m_QuadTransformDataCount++;
		m_QuadIndexCount += 6;

		m_Statistics.QuadCount++;
	}

	void Renderer2D::SubmitQuad(const glm::vec2& position, const glm::vec2& size, Texture* texture, const glm::vec4& color)
	{
		SubmitQuad({ position.x, position.y, 0.0f }, { size.x, size.y, 1.0f }, texture, color);
	}

	void Renderer2D::SubmitQuad(const glm::vec3& position, const glm::vec3& size, Texture* texture, const glm::vec4& color)
	{
		auto& quadVertexBufferPtr = GetWriteableQuadBuffer();
		auto& quadTransformDataPtr = GetQuadTransformDataPtr();
		uint32_t textureIndex = GetTextureIndex(texture);

		for (size_t i = 0; i < 4; i++)
		{
			quadVertexBufferPtr->VertexPosition = m_QuadVertexPositions[i];
			quadVertexBufferPtr->Color = color;
			quadVertexBufferPtr->TexCoord = m_QuadVertexTexCoords[i];
			quadVertexBufferPtr->TextureIndex = textureIndex;
			quadVertexBufferPtr->QuadIndex = m_QuadTransformDataCount;
			quadVertexBufferPtr++;
		}

		quadTransformDataPtr.Position = position;
		quadTransformDataPtr.Scale = size;

		m_QuadTransformDataCount++;
		m_QuadIndexCount += 6;

		m_Statistics.QuadCount++;
	}

	void Renderer2D::SubmitLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color, float thickness)
	{
		SubmitLine({ start.x, start.y, 0.0f }, { end.x, end.y, 0.0f }, color, thickness);
	}

	void Renderer2D::SubmitLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color, float thickness)
	{
		auto& lineVertexBufferPtr = GetWriteableLinePtr();

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

			lineVertexBufferPtr->Position = vertex;
			lineVertexBufferPtr->Color = color;
			lineVertexBufferPtr++;
		}

		m_LineIndexCount += 6;

		m_Statistics.LineCount++;
	}

	bool Renderer2D::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		m_QuadRenderPass->Resize(event.GetWidth(), event.GetHeight());
		m_LineRenderPass->Resize(event.GetWidth(), event.GetHeight());

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
			vertexBufferCreateInfo.Size = sizeof(Renderer2D::QuadVertex) * m_Capabilities.MaxQuadVertices;
			newVertexBuffers[i] = VertexBuffer::Create(vertexBufferCreateInfo);
			newVertexBufferBases[i] = new QuadVertex[m_Capabilities.MaxQuadVertices];
		}
	}

	Renderer2D::QuadVertex*& Renderer2D::GetWriteableQuadBuffer()
	{
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

		m_QuadBufferWriteIndex = m_QuadIndexCount / m_Capabilities.MaxQuadIndices;
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

	void Renderer2D::AddLineVertexBuffer()
	{
		uint32_t framesInFlight = Renderer::GetFramesInFlight();

		std::vector<VertexBuffer*>& newVertexBuffers = m_LineVertexBuffers.emplace_back();
		std::vector<LineVertex*>& newVertexBufferBases = m_LineVertexBufferBases.emplace_back();

		newVertexBuffers.resize(framesInFlight);
		newVertexBufferBases.resize(framesInFlight);
		for (uint32_t i = 0; i < framesInFlight; i++)
		{
			VertexBufferCreateInfo vertexBufferCreateInfo{};
			vertexBufferCreateInfo.Usage = Enumerations::BufferUsageFlags::VertexBuffer;
			vertexBufferCreateInfo.Size = sizeof(Renderer2D::LineVertex) * m_Capabilities.MaxQuadVertices;
			newVertexBuffers[i] = VertexBuffer::Create(vertexBufferCreateInfo);
			newVertexBufferBases[i] = new LineVertex[m_Capabilities.MaxQuadVertices];
		}
	}

	Renderer2D::LineVertex*& Renderer2D::GetWriteableLinePtr()
	{
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

		m_LineBufferWriteIndex = m_LineIndexCount / m_Capabilities.MaxQuadIndices;
		if (m_LineBufferWriteIndex >= m_LineVertexBufferBases.size())
		{
			AddLineVertexBuffer();
			m_LineVertexBufferPtr.emplace_back();
			m_LineVertexBufferPtr[m_LineBufferWriteIndex] = m_LineVertexBufferBases[m_LineBufferWriteIndex][frameIndex];
		}

		return m_LineVertexBufferPtr[m_LineBufferWriteIndex];
	}

	uint32_t Renderer2D::GetTextureIndex(Texture* texture)
	{
		uint32_t textureIndex = 0;
		for (uint32_t i = 1; i < m_TextureSlotIndex; i++)
		{
			if (m_TextureSlots[i] == texture)
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == 0)
		{
			//if (m_TextureSlotIndex >= MaxTextureSlots)
			//	FlushAndReset();

			textureIndex = m_TextureSlotIndex;
			m_TextureSlots[m_TextureSlotIndex] = texture;
			m_TextureSlotIndex++;
		}

		return textureIndex;
	}

	void Renderer2D::StartBatch()
	{
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

		m_QuadIndexCount = 0;
		for (uint32_t i = 0; i < m_QuadVertexBufferPtr.size(); i++)
		{
			m_QuadVertexBufferPtr[i] = m_QuadVertexBufferBases[i][frameIndex];
		}

		m_TextureSlotIndex = 1;
		for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
		{
			m_TextureSlots[i] = nullptr;
		}

		m_QuadTransformDataCount = 0;

		m_LineIndexCount = 0;
		for (uint32_t i = 0; i < m_LineVertexBufferPtr.size(); i++)
		{
			m_LineVertexBufferPtr[i] = m_LineVertexBufferBases[i][frameIndex];
		}
	}

	void Renderer2D::Flush()
	{
		CommandBuffer* drawCommandBuffer = Renderer::GetDrawCommandBuffer();
		RenderCommand* renderCommand = Renderer::GetRenderCommand();
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

		{	// Draw quads
			renderCommand->BeginRenderPass(drawCommandBuffer, m_QuadRenderPass, frameIndex);

			m_CommandBuffer->Begin(m_QuadRenderPass, frameIndex);

			renderCommand->SetViewport(m_CommandBuffer, m_QuadRenderPass, frameIndex);
			for (uint32_t i = 0; i <= m_QuadBufferWriteIndex; i++)
			{
				uint32_t dataSize = (uint32_t)((uint8_t*)m_QuadVertexBufferPtr[i] - (uint8_t*)m_QuadVertexBufferBases[i][frameIndex]);
				if (dataSize)
				{
					m_QuadVertexBuffers[i][frameIndex]->Upload(dataSize, m_QuadVertexBufferBases[i][frameIndex]);

					uint32_t indexCount = i == m_QuadBufferWriteIndex ? m_QuadIndexCount - (m_Capabilities.MaxQuadIndices * i) : m_Capabilities.MaxQuadIndices;

					renderCommand->DrawIndexed(m_CommandBuffer, m_QuadPipeline, m_QuadVertexBuffers[i][frameIndex], m_QuadIndexBuffer, indexCount, frameIndex);

					m_Statistics.DrawCalls++;
				}
			}

			renderCommand->EndRenderPass(drawCommandBuffer, frameIndex);
		}

		{	// Draw lines
			renderCommand->BeginRenderPass(drawCommandBuffer, m_LineRenderPass, frameIndex);

			renderCommand->SetViewport(m_CommandBuffer, m_LineRenderPass, frameIndex);
			for (uint32_t i = 0; i <= m_LineBufferWriteIndex; i++)
			{
				uint32_t dataSize = (uint32_t)((uint8_t*)m_LineVertexBufferPtr[i] - (uint8_t*)m_LineVertexBufferBases[i][frameIndex]);
				if (dataSize)
				{
					m_LineVertexBuffers[i][frameIndex]->Upload(dataSize, m_LineVertexBufferBases[i][frameIndex]);

					uint32_t indexCount = i == m_LineBufferWriteIndex ? m_LineIndexCount - (m_Capabilities.MaxQuadIndices * i) : m_Capabilities.MaxQuadIndices;

					renderCommand->DrawIndexed(m_CommandBuffer, m_LinePipeline, m_LineVertexBuffers[i][frameIndex], m_LineIndexBuffer, indexCount, frameIndex);

					m_Statistics.DrawCalls++;
				}
			}

			m_CommandBuffer->End(frameIndex);
			m_CommandBuffer->Execute(drawCommandBuffer, frameIndex);

			renderCommand->EndRenderPass(drawCommandBuffer, frameIndex);
		}
	}

	void Renderer2D::NextBatch()
	{
		Flush();
		StartBatch();
	}

}
