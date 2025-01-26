#pragma once
#include "Atom/Graphics/VertexAttributeLayout.h"

namespace Atom
{

	class Shader;
	class RenderPass;
	class UniformBuffer;
	class StorageBuffer;
	class Texture;

	struct PipelineOptions
	{
		VertexAttributeLayout Layout;
		Shader* Shader;
		RenderPass* RenderPass;
		UniformBuffer* UniformBuffer;
		StorageBuffer* StorageBuffer;
		Texture* Texture;
	};

	class Pipeline
	{
	public:
		static Pipeline* Create(const PipelineOptions& options);
	public:
		Pipeline(const PipelineOptions& options);
		virtual ~Pipeline() = default;
	protected:
		PipelineOptions m_Options;
	};

}
