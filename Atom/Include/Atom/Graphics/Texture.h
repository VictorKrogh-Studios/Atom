#pragma once

namespace Atom
{

	class Texture
	{
	public:
		static Texture* Create(const std::filesystem::path& filepath);
	public:
		Texture(uint32_t width, uint32_t height, uint32_t channels);
		virtual ~Texture() = default;
	protected:
		uint32_t m_Width;
		uint32_t m_Height;
		uint32_t m_Channels;
	};

}
