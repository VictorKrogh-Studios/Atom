#pragma once
#include "Atom/Graphics/Enumerations/ImageFormat.h"
#include "Atom/Graphics/VertexAttributeLayout.h"

namespace Atom
{

	class Shader;

	struct PipelineOptions
	{
		Enumerations::ImageFormat ImageFormat;
		Shader* Shader;
		VertexAttributeLayout Layout;
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
