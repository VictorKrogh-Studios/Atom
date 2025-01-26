#include "ATPCH.h"
#include "Atom/Graphics/Texture.h"

#include "Atom/Graphics/Vulkan/VulkanTexture.h"

namespace Atom
{

	Texture* Texture::Create(const std::filesystem::path& filepath)
	{
		return new VulkanTexture(filepath);
	}

	Texture::Texture(uint32_t width, uint32_t height, uint32_t channels)
		: m_Width(width), m_Height(height), m_Channels(channels)
	{
	}

}