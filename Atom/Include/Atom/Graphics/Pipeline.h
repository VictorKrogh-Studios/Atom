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

		virtual void Set(uint32_t binding, StorageBuffer* storageBuffer) const = 0;
		virtual void Set(uint32_t binding, Texture* texture, uint32_t slot) const = 0;
	protected:
		PipelineOptions m_Options;
	};

}
