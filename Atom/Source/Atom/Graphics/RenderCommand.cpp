#include "ATPCH.h"
#include "Atom/Graphics/RenderCommand.h"

#include "Atom/Graphics/Renderer.h"

#include "Atom/Graphics/Vulkan/VulkanRenderCommand.h"

namespace Atom
{

	RenderCommand* RenderCommand::Create()
	{
		return new VulkanRenderCommand();
	}

}