#include "ATPCH.h"
#include "Pipeline.h"

#include "Atom/Graphics/Vulkan/VulkanPipeline.h"

namespace Atom
{

	Pipeline* Pipeline::Create(const PipelineOptions& options)
	{
		return new VulkanPipeline(options);
	}

	Pipeline::Pipeline(const PipelineOptions& options)
		: m_Options(options)
	{
	}

}