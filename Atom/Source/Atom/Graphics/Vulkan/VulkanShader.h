#pragma once
#include "Atom/Graphics/Shader.h"
#include "Atom/Graphics/Enumerations/ShaderType.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& name, const std::filesystem::path& filepath);
		VulkanShader(const std::filesystem::path& filepath);
		virtual ~VulkanShader();
	private:
		std::string ReadFile(const std::filesystem::path& filepath) const;
		std::unordered_map<Enumerations::ShaderType, std::string> PreProcess(const std::string& source) const;
		std::unordered_map<Enumerations::ShaderType, std::vector<uint32_t>> CompileVulkanSpvBinaries(const std::unordered_map<Enumerations::ShaderType, std::string>& sources) const;


		VkShaderModule CreateShaderModule(const std::vector<uint32_t>& shaderCode);
		std::vector<VkPipelineShaderStageCreateInfo> CreatePipelineShaderStageCreateInfos() const;
	private:
		std::unordered_map<Enumerations::ShaderType, std::vector<uint32_t>> m_SPIRVData;

		VkShaderModule m_VertexShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_FragmentShaderModule = VK_NULL_HANDLE;

		std::vector<VkPipelineShaderStageCreateInfo> m_PipelineShaderStageCreateInfos;

		friend class VulkanPipeline;
	};

}
