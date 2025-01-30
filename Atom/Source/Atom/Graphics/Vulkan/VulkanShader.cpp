#include "ATPCH.h"
#include "VulkanShader.h"
#include "VulkanGraphicsContext.h"

#include "Atom/Graphics/Vulkan/VulkanUniformBuffer.h"
#include "Atom/Graphics/Vulkan/VulkanStorageBuffer.h"
#include "Atom/Graphics/Vulkan/VulkanTexture.h"

#include "Atom/Core/Timer.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_cross.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Atom
{

	VulkanShader::VulkanShader(const std::string& name, const std::filesystem::path& filepath)
		: Shader(name, filepath)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		std::string source = ReadFile(filepath);

		ShaderCompilerOptions shaderCompilerOptions{};
		shaderCompilerOptions.GenerateDebugInfo = true;
		ShaderCompiler compiler(shaderCompilerOptions);
		auto result = compiler.CompileAndReflect(m_Name, source);

		std::vector<ShaderDescriptorData> shaderDescriptors;
		shaderDescriptors.reserve(result.size());

		for (auto& [type, result] : result)
		{
			shaderDescriptors.push_back(ShaderDescriptorData{
				result.SPIRVData,
				result.ReflectionData,
				type
				});
		}

		CreateDescriptorSetLayout(device, shaderDescriptors);
		CreateDescriptorPool(device, shaderDescriptors);
		CreateDescriptorSets(device, shaderDescriptors);

		m_VertexShaderModule = CreateShaderModule(result[shaderc_vertex_shader].SPIRVData);
		m_FragmentShaderModule = CreateShaderModule(result[shaderc_fragment_shader].SPIRVData);

		m_PipelineShaderStageCreateInfos = CreatePipelineShaderStageCreateInfos();

		// TODO: Cache SPIRV binaries
	}

	VulkanShader::VulkanShader(const std::filesystem::path& filepath)
		: Shader(GetFileName(filepath), filepath)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		std::string source = ReadFile(filepath);

		ShaderCompilerOptions shaderCompilerOptions{};
		shaderCompilerOptions.GenerateDebugInfo = true;
		ShaderCompiler compiler(shaderCompilerOptions);
		auto result = compiler.CompileAndReflect(m_Name, source);

		std::vector<ShaderDescriptorData> shaderDescriptors;
		shaderDescriptors.reserve(result.size());

		for (auto& [type, result] : result)
		{
			shaderDescriptors.push_back(ShaderDescriptorData{
				result.SPIRVData,
				result.ReflectionData,
				type
				});
		}

		CreateDescriptorSetLayout(device, shaderDescriptors);
		CreateDescriptorPool(device, shaderDescriptors);
		CreateDescriptorSets(device, shaderDescriptors);

		m_VertexShaderModule = CreateShaderModule(result[shaderc_vertex_shader].SPIRVData);
		m_FragmentShaderModule = CreateShaderModule(result[shaderc_fragment_shader].SPIRVData);

		m_PipelineShaderStageCreateInfos = CreatePipelineShaderStageCreateInfos();

		// TODO: Cache SPIRV binaries
	}

	VulkanShader::~VulkanShader()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);

		vkDestroyShaderModule(device, m_VertexShaderModule, nullptr);
		vkDestroyShaderModule(device, m_FragmentShaderModule, nullptr);
	}

	void VulkanShader::SetUniformBuffer(uint32_t binding, UniformBuffer* uniformBuffer) const
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		VulkanUniformBuffer* vulkanUniformBuffer = static_cast<VulkanUniformBuffer*>(uniformBuffer);

		for (size_t i = 0; i < 3; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = vulkanUniformBuffer->m_Buffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = vulkanUniformBuffer->GetSize();

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSets[i];
			descriptorWrite.dstBinding = binding; 
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr; // Optional
			descriptorWrite.pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		}
	}

	void VulkanShader::SetStorageBuffer(uint32_t binding, StorageBuffer* storageBuffer) const
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		VulkanStorageBuffer* vulkanStorageBuffer = static_cast<VulkanStorageBuffer*>(storageBuffer);

		for (size_t i = 0; i < 3; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = vulkanStorageBuffer->m_Buffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = vulkanStorageBuffer->GetSize();

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSets[i];
			descriptorWrite.dstBinding = binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr; // Optional
			descriptorWrite.pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		}
	}

	void VulkanShader::SetTexture(uint32_t binding, Texture* texture) const
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

		VulkanTexture* vulkanTexture = static_cast<VulkanTexture*>(texture);

		for (size_t i = 0; i < 3; i++)
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = vulkanTexture->m_ImageView;
			imageInfo.sampler = vulkanTexture->m_Sampler;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSets[i];
			descriptorWrite.dstBinding = binding;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = nullptr; // Optional
			descriptorWrite.pImageInfo = &imageInfo;
			descriptorWrite.pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		}
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

	inline static VkShaderStageFlags GetShaderStageByShaderKind(shaderc_shader_kind shaderKind)
	{
		switch (shaderKind)
		{
			case shaderc_vertex_shader:  return VK_SHADER_STAGE_VERTEX_BIT;
			case shaderc_fragment_shader:  return VK_SHADER_STAGE_FRAGMENT_BIT;
			case shaderc_anyhit_shader:
			default: break;
		}

		AT_CORE_ASSERT(false, "Failed to determine Vk Shader Stage flag");
		return VK_SHADER_STAGE_ALL_GRAPHICS;
	}

	void VulkanShader::CreateDescriptorSetLayout(VkDevice device, const std::vector<ShaderDescriptorData>& shaderDescriptors)
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		for (const auto& shaderDescriptor : shaderDescriptors)
		{
			for (const auto& uniformBufferDescriptor : shaderDescriptor.ReflectionData.UniformBuffers)
			{
				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
				descriptorSetLayoutBinding.binding = uniformBufferDescriptor.Binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = GetShaderStageByShaderKind(shaderDescriptor.ShaderKind);
				descriptorSetLayoutBinding.pImmutableSamplers = nullptr; // Optional

				bindings.push_back(descriptorSetLayoutBinding);
			}

			for (const auto& storageBufferDescriptor : shaderDescriptor.ReflectionData.StorageBuffers)
			{
				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
				descriptorSetLayoutBinding.binding = storageBufferDescriptor.Binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = GetShaderStageByShaderKind(shaderDescriptor.ShaderKind);
				descriptorSetLayoutBinding.pImmutableSamplers = nullptr; // Optional

				bindings.push_back(descriptorSetLayoutBinding);
			}

			for (const auto& imageSamplerDescriptor : shaderDescriptor.ReflectionData.SampledImages)
			{
				VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{};
				descriptorSetLayoutBinding.binding = imageSamplerDescriptor.Binding;
				descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorSetLayoutBinding.descriptorCount = 1;
				descriptorSetLayoutBinding.stageFlags = GetShaderStageByShaderKind(shaderDescriptor.ShaderKind);
				descriptorSetLayoutBinding.pImmutableSamplers = nullptr; // Optional

				bindings.push_back(descriptorSetLayoutBinding);
			}
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Descriptor Set Layout");
	}

	void VulkanShader::CreateDescriptorPool(VkDevice device, const std::vector<ShaderDescriptorData>& shaderDescriptors)
	{
		std::vector<VkDescriptorPoolSize> descriptorPoolSizes;

		for (const auto& shaderDescriptor : shaderDescriptors)
		{
			if (!shaderDescriptor.ReflectionData.UniformBuffers.empty())
			{
				descriptorPoolSizes.push_back(VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, (uint32_t)(shaderDescriptor.ReflectionData.UniformBuffers.size() * 3) }); // 3 = Frames in Flight
			}

			if (!shaderDescriptor.ReflectionData.StorageBuffers.empty())
			{
				descriptorPoolSizes.push_back(VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, (uint32_t)(shaderDescriptor.ReflectionData.StorageBuffers.size() * 3) }); // 3 = Frames in Flight
			}

			if (!shaderDescriptor.ReflectionData.SampledImages.empty())
			{
				descriptorPoolSizes.push_back(VkDescriptorPoolSize{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, (uint32_t)(shaderDescriptor.ReflectionData.SampledImages.size() * 3) }); // 3 = Frames in Flight
			}
		}

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
		descriptorPoolCreateInfo.maxSets = 3; // 3 = Frames in Flight

		VkResult result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Descriptor Pool");
	}

	void VulkanShader::CreateDescriptorSets(VkDevice device, const std::vector<ShaderDescriptorData>& shaderDescriptors)
	{
		std::vector<VkDescriptorSetLayout> layouts(3, m_DescriptorSetLayout);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size()); // 3 = Frames in Flight
		allocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(3); // 3 = Frames in Flight
		VkResult result = vkAllocateDescriptorSets(device, &allocInfo, m_DescriptorSets.data());
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate Descriptor Sets");
	}

	VkShaderModule VulkanShader::CreateShaderModule(const std::vector<uint32_t>& shaderCode)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->GetVkDevice();

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