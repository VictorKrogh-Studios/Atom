#include "ATPCH.h"
#include "Atom/Graphics/ShaderLibrary.h"

namespace Atom
{
	
	ShaderLibrary::~ShaderLibrary()
	{
		for (auto&& [name, shader] : m_Shaders)
		{
			delete shader;
		}

		m_Shaders.clear();
	}

	void ShaderLibrary::Add(Shader* shader)
	{
		Add(shader->GetName(), shader);
	}

	void ShaderLibrary::Add(const std::string& name, Shader* shader)
	{
		AT_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());

		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Load(const std::filesystem::path& filepath)
	{
		const std::string& filename = GetFileName(filepath);

		Load(filename, filepath);
	}

	void ShaderLibrary::Load(const std::string& name, const std::filesystem::path& filepath)
	{
		AT_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());

		m_Shaders[name] = Shader::CreateFromFile(name, filepath);
	}

	Shader* ShaderLibrary::Get(const std::string& name) const
	{
		AT_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end());

		return m_Shaders.at(name);
	}

	std::string ShaderLibrary::GetFileName(const std::filesystem::path& filepath) const
	{
		const std::string& filepathString = filepath.string();

		// Extract name from filepath
		auto lastSlash = filepathString.find_last_of("/\\");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepathString.rfind('.');
		auto count = lastDot == std::string::npos ? filepathString.size() - lastSlash : lastDot - lastSlash;
		return filepathString.substr(lastSlash, count);
	}

}