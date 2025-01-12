#pragma once
#include "Atom/Graphics/Shader.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& vertexFilePath, const std::string& fragmentFilePath);
		~VulkanShader();
	private:
		VkShaderModule CreateShaderModule(const std::vector<char>& shaderCode);
		std::vector<VkPipelineShaderStageCreateInfo> CreatePipelineShaderStageCreateInfos() const;

		const std::vector< VkPipelineShaderStageCreateInfo> GetPipelineShaderStageCreateInfos() const { return m_PipelineShaderStageCreateInfos; }
	private:
		VkShaderModule m_VertexShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_FragmentShaderModule = VK_NULL_HANDLE;

		std::vector<VkPipelineShaderStageCreateInfo> m_PipelineShaderStageCreateInfos;
	};

}
