#pragma once

namespace Atom
{

	class UniformBuffer;
	class StorageBuffer;
	class Texture;

	class Shader
	{
	public:
		static Shader* CreateFromFile(const std::string& name, const std::filesystem::path& filepath);
		static Shader* CreateFromFile(const std::filesystem::path& filepath);
	public:
		Shader(const std::string& name, const std::filesystem::path& filepath);
		virtual ~Shader() = default;

		virtual void Set(uint32_t binding, UniformBuffer* uniformBuffer) const = 0;
		virtual void Set(uint32_t binding, StorageBuffer* storageBuffer) const = 0;
		virtual void Set(uint32_t binding, Texture* texture) const = 0;
		virtual void Set(uint32_t binding, Texture* texture, uint32_t slot) const = 0;

		const std::string& GetName() const { return m_Name; }
	protected:
		virtual std::vector<char> ReadSpvFile(const std::string& filepath) const;
		virtual std::string GetFileName(const std::filesystem::path& filepath) const;
	protected:
		std::string m_Name;
		std::filesystem::path m_FilePath;
	};

}
