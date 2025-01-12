#pragma once
#include "Atom/Graphics/Enumerations/ImageFormat.h"

namespace Atom
{

	class Shader;

	struct PipelineOptions
	{
		Enumerations::ImageFormat ImageFormat;
		Shader* Shader;
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
