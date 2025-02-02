#include "ATPCH.h"
#include "Atom/Graphics/Texture.h"

#include "Atom/Graphics/Vulkan/VulkanTexture.h"

namespace Atom
{

	Texture* Texture::Create(const std::filesystem::path& filepath, const TextureCreateInfo& createInfo)
	{
		return new VulkanTexture(filepath, createInfo);
	}

	Texture* Texture::Create(uint32_t width, uint32_t height, void* data, const TextureCreateInfo& createInfo)
	{
		return new VulkanTexture(width, height, data, createInfo);
	}

	Texture::Texture(uint32_t width, uint32_t height, uint32_t channels, const TextureCreateInfo& createInfo)
		: m_Width(width), m_Height(height), m_Channels(channels), m_CreateInfo(createInfo)
	{
	}

}