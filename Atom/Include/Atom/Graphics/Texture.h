#pragma once

namespace Atom
{

	struct TextureCreateInfo
	{
	};

	typedef void* TextureNativeHandle;

	class Texture
	{
	public:
		static Texture* Create(const std::filesystem::path& filepath, const TextureCreateInfo& createInfo = TextureCreateInfo());
		static Texture* Create(uint32_t width, uint32_t height, void* data, const TextureCreateInfo& createInfo = TextureCreateInfo());
	public:
		Texture(uint32_t width, uint32_t height, uint32_t channels, const TextureCreateInfo& createInfo);
		virtual ~Texture() = default;

		// Return ImGui handle, for rendering textures - using ImGui::Image()
		virtual TextureNativeHandle GetNativeHandle() = 0; 
	protected:
		TextureCreateInfo m_CreateInfo;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;
	};

}
