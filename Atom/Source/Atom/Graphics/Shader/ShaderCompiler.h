#pragma once
#include "Atom/Graphics/Enumerations/ShaderType.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>

namespace Atom
{

	struct ShaderCompilerOptions
	{
		bool GenerateDebugInfo = false;
		bool Optimize = true;
		shaderc_optimization_level OptimizationLevel = shaderc_optimization_level_performance;
	};

	struct ShaderDescriptorReflectionData
	{
		std::string Name;
		uint32_t Binding;
		uint32_t DescriptorSet;
	};

	struct ShaderBufferReflectionData : public ShaderDescriptorReflectionData
	{
		uint32_t Size;
		uint32_t MemberCount;
	};

	struct UniformBufferReflectionData : public ShaderBufferReflectionData
	{
	};

	struct StorageBufferReflectionData : public ShaderBufferReflectionData
	{
	};

	struct SampledImageReflectionData : public ShaderDescriptorReflectionData
	{
		uint32_t Dimension;
		uint32_t ArraySize;
	};

	struct ShaderReflectionData
	{
		std::vector<UniformBufferReflectionData> UniformBuffers;
		std::vector<StorageBufferReflectionData> StorageBuffers;
		std::vector<SampledImageReflectionData> SampledImages;
	};

	struct ShaderCompilationResult
	{
		std::vector<uint32_t> SPIRVData;
		ShaderReflectionData ReflectionData;
	};

	class ShaderCompiler
	{
	public:
		ShaderCompiler(const ShaderCompilerOptions& options = {});
		~ShaderCompiler() = default;

		std::unordered_map<Enumerations::ShaderType, ShaderCompilationResult> CompileAndReflect(const std::string& name, const std::string& source) const;

		std::unordered_map<Enumerations::ShaderType, std::string> PreProcess(const std::string& source) const;
		std::unordered_map<Enumerations::ShaderType, std::vector<uint32_t>> CompileSpvBinaries(const std::string& name, const std::unordered_map<Enumerations::ShaderType, std::string>& sources) const;
		std::unordered_map<Enumerations::ShaderType, ShaderReflectionData> ReflectSpvBinaries(const std::unordered_map<Enumerations::ShaderType, std::vector<uint32_t>>& spirvData) const;
		ShaderReflectionData Reflect(const std::vector<uint32_t>& data) const;
	private:
		ShaderCompilerOptions m_Options;
	};

}
