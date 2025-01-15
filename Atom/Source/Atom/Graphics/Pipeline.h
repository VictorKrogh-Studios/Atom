#pragma once
//#include "Atom/Graphics/Enumerations/ImageFormat.h"
#include "Atom/Graphics/VertexAttributeLayout.h"

namespace Atom
{

	class Shader;
	class RenderPass;

	struct PipelineOptions
	{
		//Enumerations::ImageFormat ImageFormat;
		VertexAttributeLayout Layout;
		Shader* Shader;
		RenderPass* RenderPass;
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
