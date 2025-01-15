#include "ATPCH.h"
#include "RenderCommand.h"

#include "Atom/Graphics/Renderer.h"

#include "Atom/Graphics/Vulkan/VulkanRenderCommand.h"

namespace Atom
{

	RenderCommand* RenderCommand::Create()
	{
		return new VulkanRenderCommand();
	}

}