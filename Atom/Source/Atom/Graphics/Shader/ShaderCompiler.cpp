#include "ATPCH.h"
#include "ShaderCompiler.h"

#include "Atom/Core/Timer.h"

namespace Atom
{

	namespace Utils
	{

		inline static Enumerations::ShaderType GetShaderTypeFromString(const std::string& typeString)
		{
			if (typeString == "vertex")
			{
				return Enumerations::ShaderType::Vertex;
			}
			if (typeString == "fragment" || typeString == "pixel")
			{
				return Enumerations::ShaderType::Fragment;
			}

			AT_CORE_ASSERT(false, "Unknown shader type");
			return Enumerations::ShaderType::None;
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

	}

	ShaderCompiler::ShaderCompiler(const ShaderCompilerOptions& options)
		: m_Options(options)
	{
	}

	std::unordered_map<Enumerations::ShaderType, ShaderCompilationResult> ShaderCompiler::CompileAndReflect(const std::string& name, const std::string& source) const
	{
		std::unordered_map<Enumerations::ShaderType, std::string> sources = PreProcess(source);

		std::unordered_map<Enumerations::ShaderType, std::vector<uint32_t>> spirvData = CompileSpvBinaries(name, sources);

		std::unordered_map<Enumerations::ShaderType, ShaderReflectionData> reflectionData = ReflectSpvBinaries(spirvData);
		
		std::unordered_map<Enumerations::ShaderType, ShaderCompilationResult> result;

		for (auto& [type, _] : sources)
		{
			result[type] = { spirvData[type], reflectionData[type] };
		}

		return result;
	}

	std::unordered_map<Enumerations::ShaderType, std::string> ShaderCompiler::PreProcess(const std::string& source) const
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

			Enumerations::ShaderType shaderType = Utils::GetShaderTypeFromString(type);
			AT_CORE_ASSERT(shaderType != Enumerations::ShaderType::None, "Invalid shader type specified");

			uint64_t nextLinePos = source.find_first_not_of("\r\n", eol);
			AT_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");
			pos = source.find(typeToken, nextLinePos);

			result[shaderType] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);
		}

		return result;
	}

	std::unordered_map<Enumerations::ShaderType, std::vector<uint32_t>> ShaderCompiler::CompileSpvBinaries(const std::string& name, const std::unordered_map<Enumerations::ShaderType, std::string>& sources) const
	{
		Timer timer;

		std::unordered_map<Enumerations::ShaderType, std::vector<uint32_t>> result;

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_2);
		if (m_Options.Optimize)
		{
			options.SetOptimizationLevel(m_Options.OptimizationLevel);
		}

		if (m_Options.GenerateDebugInfo)
		{
			options.SetGenerateDebugInfo();
		}

		options.SetSourceLanguage(shaderc_source_language_glsl);

		for (auto&& [type, source] : sources)
		{
			shaderc::SpvCompilationResult compilationResult = compiler.CompileGlslToSpv(source, Utils::GetShaderKindByShaderType(type), name.c_str(), options);
			if (compilationResult.GetCompilationStatus() != shaderc_compilation_status_success)
			{
				AT_CORE_ERROR(compilationResult.GetErrorMessage());
				AT_CORE_ASSERT(false);
			}

			result[type] = std::vector<uint32_t>(compilationResult.begin(), compilationResult.end());
		}

		AT_CORE_TRACE("Successfully compiled shader '{0}', took {1}ms", name, timer.ElapsedMilliseconds());

		return result;
	}

	std::unordered_map<Enumerations::ShaderType, ShaderReflectionData> ShaderCompiler::ReflectSpvBinaries(const std::unordered_map<Enumerations::ShaderType, std::vector<uint32_t>>& spirvData) const
	{
		std::unordered_map<Enumerations::ShaderType, ShaderReflectionData> result;

		for (auto& [type, data] : spirvData)
		{
			result[type] = Reflect(data);
		}

		return result;
	}

	ShaderReflectionData ShaderCompiler::Reflect(const std::vector<uint32_t>& data) const
	{
		ShaderReflectionData reflectionData;

		spirv_cross::Compiler compiler(data);
		spirv_cross::ShaderResources resources = compiler.get_shader_resources();

		for (const auto& resource : resources.uniform_buffers)
		{
			auto activeUniformBuffers = compiler.get_active_buffer_ranges(resource.id);

			if (activeUniformBuffers.size())
			{
				const auto& name = resource.name;
				auto& bufferType = compiler.get_type(resource.base_type_id);
				uint32_t memberCount = (uint32_t)bufferType.member_types.size();
				uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32_t size = (uint32_t)compiler.get_declared_struct_size(bufferType);

				reflectionData.UniformBuffers.push_back(UniformBufferReflectionData{
					name,
					binding,
					descriptorSet,
					size,
					memberCount
					});
			}
		}

		for (const auto& resource : resources.storage_buffers)
		{
			auto activeUniformBuffers = compiler.get_active_buffer_ranges(resource.id);

			if (activeUniformBuffers.size())
			{
				const auto& name = resource.name;
				auto& bufferType = compiler.get_type(resource.base_type_id);
				uint32_t memberCount = (uint32_t)bufferType.member_types.size();
				uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
				uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
				uint32_t size = (uint32_t)compiler.get_declared_struct_size(bufferType);

				reflectionData.StorageBuffers.push_back(StorageBufferReflectionData{
					name,
					binding,
					descriptorSet,
					size,
					memberCount
					});
			}
		}

		for (const auto& resource : resources.sampled_images)
		{
			const auto& name = resource.name;
			auto& baseType = compiler.get_type(resource.base_type_id);
			auto& type = compiler.get_type(resource.type_id);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t dimension = baseType.image.dim;
			uint32_t arraySize = type.array.size() > 0 ? type.array[0] : 1;

			reflectionData.SampledImages.push_back(SampledImageReflectionData{
				name,
				binding,
				descriptorSet,
				dimension,
				arraySize
				});
		}

		return reflectionData;
	}

}