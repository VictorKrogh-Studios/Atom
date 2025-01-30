#include "ATPCH.h"
#include "Atom/Graphics/Shader.h"

#include "Vulkan/VulkanShader.h"

namespace Atom
{

	Shader* Shader::CreateFromFile(const std::string& name, const std::filesystem::path& filepath)
	{
		return new VulkanShader(name, filepath);
	}

	Shader* Shader::CreateFromFile(const std::filesystem::path& filepath)
	{
		return new VulkanShader(filepath);
	}

	Shader::Shader(const std::string& name, const std::filesystem::path& filepath)
		: m_Name(name), m_FilePath(filepath)
	{
	}

	std::vector<char> Shader::ReadSpvFile(const std::string& filepath) const
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);
		AT_CORE_ASSERT(file.is_open(), "Failed to open file");

		size_t size = (size_t)file.tellg();
		std::vector<char> buffer(size);

		file.seekg(0);
		file.read(buffer.data(), size);

		file.close();

		return buffer;
	}

	std::string Shader::GetFileName(const std::filesystem::path& filepath) const
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
