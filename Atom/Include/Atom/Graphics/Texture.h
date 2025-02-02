#pragma once

namespace Atom
{

	struct TextureCreateInfo
	{
	};

	class Texture
	{
	public:
		static Texture* Create(const std::filesystem::path& filepath, const TextureCreateInfo& createInfo = TextureCreateInfo());
		static Texture* Create(uint32_t width, uint32_t height, void* data, const TextureCreateInfo& createInfo = TextureCreateInfo());
	public:
		Texture(uint32_t width, uint32_t height, uint32_t channels, const TextureCreateInfo& createInfo);
		virtual ~Texture() = default;
	protected:
		TextureCreateInfo m_CreateInfo;
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;
	};

	class RenderTexture
	{
	public:
		virtual ~RenderTexture() = default;

		virtual void* GetRaw() const = 0;
	};

}
