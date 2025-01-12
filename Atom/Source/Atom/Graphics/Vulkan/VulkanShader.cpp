#include "ATPCH.h"
#include "VulkanShader.h"
#include "VulkanGraphicsContext.h"

namespace Atom
{

	VulkanShader::VulkanShader(const std::string& vertexFilePath, const std::string& fragmentFilePath)
	{
		std::vector<char> vertexShaderCode = ReadSpvFile(vertexFilePath);
		std::vector<char> fragmentShaderCode = ReadSpvFile(fragmentFilePath);

		m_VertexShaderModule = CreateShaderModule(vertexShaderCode);
		m_FragmentShaderModule = CreateShaderModule(fragmentShaderCode);

		m_PipelineShaderStageCreateInfos = CreatePipelineShaderStageCreateInfos();
	}

	VulkanShader::~VulkanShader()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		vkDestroyShaderModule(device, m_VertexShaderModule, nullptr);
		vkDestroyShaderModule(device, m_FragmentShaderModule, nullptr);
	}

	VkShaderModule VulkanShader::CreateShaderModule(const std::vector<char>& shaderCode)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

		VkShaderModule shaderModule = VK_NULL_HANDLE;

		VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
		AT_CORE_ASSERT(result == VK_SUCCESS);

		return shaderModule;
	}

	std::vector<VkPipelineShaderStageCreateInfo> VulkanShader::CreatePipelineShaderStageCreateInfos() const
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
		shaderStageCreateInfos.resize(2);

		VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo{};
		vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageCreateInfo.module = m_VertexShaderModule;
		vertShaderStageCreateInfo.pName = "main";
		shaderStageCreateInfos[0] = vertShaderStageCreateInfo;

		VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo{};
		fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageCreateInfo.module = m_FragmentShaderModule;
		fragShaderStageCreateInfo.pName = "main";
		shaderStageCreateInfos[1] = fragShaderStageCreateInfo;

		return shaderStageCreateInfos;
	}

}