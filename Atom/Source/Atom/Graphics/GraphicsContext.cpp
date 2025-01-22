#include "ATPCH.h"
#include "Atom/Graphics/GraphicsContext.h"

#include "Atom/Graphics/Vulkan/VulkanGraphicsContext.h"

namespace Atom
{

	GraphicsContext* GraphicsContext::Create(const GraphicsContextOptions& options)
	{
		return new VulkanGraphicsContext(options);
	}

	GraphicsContext::GraphicsContext(const GraphicsContextOptions& options)
		: m_Options(options)
	{
	}

}