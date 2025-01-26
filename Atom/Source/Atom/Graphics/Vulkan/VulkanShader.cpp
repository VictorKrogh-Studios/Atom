#include "ATPCH.h"
#include "VulkanShader.h"
#include "VulkanGraphicsContext.h"

#include "Atom/Core/Timer.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Atom
{

	VulkanShader::VulkanShader(const std::string& name, const std::filesystem::path& filepath)
		: Shader(name, filepath)
	{
		std::string source = ReadFile(filepath);
		std::unordered_map<Enumerations::ShaderType, std::string> shaderSources = PreProcess(source);
		m_SPIRVData = CompileVulkanSpvBinaries(shaderSources);

		m_VertexShaderModule = CreateShaderModule(m_SPIRVData[Enumerations::ShaderType::Vertex]);
		m_FragmentShaderModule = CreateShaderModule(m_SPIRVData[Enumerations::ShaderType::Fragment]);

		m_PipelineShaderStageCreateInfos = CreatePipelineShaderStageCreateInfos();

		// TODO: Cache SPIRV binaries
		// TODO: Do SPIRV Reflection
	}

	VulkanShader::VulkanShader(const std::filesystem::path& filepath)
		: Shader(GetFileName(filepath), filepath)
	{
		std::string source = ReadFile(filepath);
		std::unordered_map<Enumerations::ShaderType, std::string> shaderSources = PreProcess(source);
		m_SPIRVData = CompileVulkanSpvBinaries(shaderSources);

		m_VertexShaderModule = CreateShaderModule(m_SPIRVData[Enumerations::ShaderType::Vertex]);
		m_FragmentShaderModule = CreateShaderModule(m_SPIRVData[Enumerations::ShaderType::Fragment]);

		m_PipelineShaderStageCreateInfos = CreatePipelineShaderStageCreateInfos();

		// TODO: Cache SPIRV binaries
		// TODO: Do SPIRV Reflection
	}

	VulkanShader::~VulkanShader()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		vkDestroyShaderModule(device, m_VertexShaderModule, nullptr);
		vkDestroyShaderModule(device, m_FragmentShaderModule, nullptr);
	}

	std::string VulkanShader::ReadFile(const std::filesystem::path& filepath) const
	{
		std::string result;

		// ifstream closes itself due to RAII
		std::ifstream stream(filepath, std::ios::in | std::ios::binary);
		if (!stream)
		{
			AT_CORE_ERROR("Failed to open file '{0}'", filepath.string());
		}

		stream.seekg(0, std::ios::end);
		uint64_t size = stream.tellg();
		if (size == -1)
		{
			AT_CORE_ERROR("Failed to read from file '{0}'", filepath.string());
		}

		result.resize(size);

		stream.seekg(0, std::ios::beg);
		stream.read(&result[0], size);

		return result;
	}

	std::unordered_map<Enumerations::ShaderType, std::string> VulkanShader::PreProcess(const std::string& source) const
	{
		std::unordered_map<Enumerations::ShaderType, std::string> result;

		const char* typeToken = "#type";
		uint64_t typeTokenLength = strlen(typeToken);
		uint64_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			uint64_t eol = source.find_first_of("\r\n", pos);
			AT_CORE_ASSERT(eol != std::string::npos, "Syntax error");

			uint64_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			AT_CORE_ASSERT(!type.empty(), "Shader type not found");

			std::transform(type.begin(), type.end(), type.begin(), [](unsigned char c)
			{
				return std::tolower(c);
			});

			Enumerations::ShaderType shaderType = GetShaderTypeFromString(type);
			AT_CORE_ASSERT(shaderType != Enumerations::ShaderType::None, "Invalid shader type specified");

			uint64_t nextLinePos = source.find_first_not_of("\r\n", eol);
			AT_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos);

			result[shaderType] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return result;
	}

	inline static shaderc_shader_kind GetShaderKindByShaderType(Enumerations::ShaderType shaderType)
	{
		switch (shaderType)
		{
			case Atom::Enumerations::ShaderType::Vertex: return shaderc_vertex_shader;
			case Atom::Enumerations::ShaderType::Fragment: return shaderc_fragment_shader;
			case Atom::Enumerations::ShaderType::None:
			default: break;
		}

		AT_CORE_ASSERT(false, "Failed to determine shaderc shader kind");
		return shaderc_anyhit_shader;
	}

	std::unordered_map<Enumerations::ShaderType, std::vector<uint32_t>> VulkanShader::CompileVulkanSpvBinaries(const std::unordered_map<Enumerations::ShaderType, std::string>& sources) const
	{
		Timer timer;

		std::unordered_map<Enumerations::ShaderType, std::vector<uint32_t>> result;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		const bool optimize = true;
		if (optimize)
		{
			options.SetOptimizationLevel(shaderc_optimization_level_performance);
		}

		for (auto&& [type, source] : sources)
		{
			shaderc::SpvCompilationResult compilationResult = compiler.CompileGlslToSpv(source, GetShaderKindByShaderType(type), m_Name.c_str(), options);
			if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				AT_CORE_ERROR(compilationResult.GetErrorMessage());
				AT_CORE_ASSERT(false);
			}

			result[type] = std::vector<uint32_t>(compilationResult.begin(), compilationResult.end());
		}

		AT_CORE_TRACE("Successfully compiled shader '{0}', took {1}ms", m_Name, timer.ElapsedMilliseconds());

		return result;
	}

	VkShaderModule VulkanShader::CreateShaderModule(const std::vector<uint32_t>& shaderCode)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.size() * sizeof(uint32_t);
		createInfo.pCode = shaderCode.data();

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