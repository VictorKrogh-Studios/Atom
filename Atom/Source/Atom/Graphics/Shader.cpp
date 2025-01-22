#include "ATPCH.h"
#include "Atom/Graphics/Shader.h"

#include "Vulkan/VulkanShader.h"

namespace Atom
{

	Shader* Shader::CreateFromFile(const std::string& vertexFilePath, const std::string& fragmentFilePath)
	{
		return new VulkanShader(vertexFilePath, fragmentFilePath);
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

}
