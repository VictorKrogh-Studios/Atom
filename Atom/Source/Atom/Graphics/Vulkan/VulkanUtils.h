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

	inline static VkFormat GetVkFormatByShaderDataType(Enumerations::ShaderDataType shaderDataType)
	{
		switch (shaderDataType)
		{
			case Atom::Enumerations::ShaderDataType::Float: return VkFormat::VK_FORMAT_R32_SFLOAT;
			case Atom::Enumerations::ShaderDataType::Float2: return VkFormat::VK_FORMAT_R32G32_SFLOAT;
			case Atom::Enumerations::ShaderDataType::Float3: return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
			case Atom::Enumerations::ShaderDataType::Float4: return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
			case Atom::Enumerations::ShaderDataType::None:
			default: break;
		}

		AT_CORE_ASSERT(true, "Unknown image format");
		return VkFormat::VK_FORMAT_UNDEFINED;
	}

}
