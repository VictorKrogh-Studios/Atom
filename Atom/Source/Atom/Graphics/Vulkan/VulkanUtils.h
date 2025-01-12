#pragma once
#include "Atom/Graphics/Enumerations/ImageFormat.h"
#include <vulkan/vulkan.h>

namespace Atom::Vulkan::Utils
{

	inline static VkFormat GetVkFormat(Enumerations::ImageFormat imageFormat)
	{
		switch (imageFormat)
		{
			case Atom::Enumerations::ImageFormat::B8G8R8A8_UNORM: return VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
			case Atom::Enumerations::ImageFormat::Unknown:
			default: break;
		}

		AT_CORE_ASSERT(true, "Unknown image format");
		return VkFormat::VK_FORMAT_UNDEFINED;
	}

}