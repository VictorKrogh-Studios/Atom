#pragma once
#include "Atom/Graphics/Enumerations/ShaderType.h"

namespace Atom
{

	class Shader
	{
	public:
		static Shader* CreateFromFile(const std::string& name, const std::filesystem::path& filepath);
		static Shader* CreateFromFile(const std::filesystem::path& filepath);
	public:
		Shader(const std::string& name, const std::filesystem::path& filepath);
		virtual ~Shader() = default;

		const std::string& GetName() const { return m_Name; }
	protected:
		virtual std::vector<char> ReadSpvFile(const std::string& filepath) const;
		virtual std::string GetFileName(const std::filesystem::path& filepath) const;

		virtual Enumerations::ShaderType GetShaderTypeFromString(const std::string& typeString) const;
	protected:
		std::string m_Name;
		std::filesystem::path m_FilePath;
	};

}
