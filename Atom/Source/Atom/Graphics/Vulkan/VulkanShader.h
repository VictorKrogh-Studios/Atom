#pragma once
#include "Atom/Graphics/Shader.h"
#include "Atom/Graphics/Shader/ShaderCompiler.h"

#include <vulkan/vulkan.h>

namespace Atom
{

	class VulkanShader : public Shader
	{
	private:
		struct ShaderDescriptorData : public ShaderCompilationResult
		{
			shaderc_shader_kind ShaderKind;
		};
	public:
		VulkanShader(const std::string& name, const std::filesystem::path& filepath);
		VulkanShader(const std::filesystem::path& filepath);
		virtual ~VulkanShader();

		virtual void Set(uint32_t binding, UniformBuffer* uniformBuffer) const override;
		virtual void Set(uint32_t binding, StorageBuffer* storageBuffer) const override;
		virtual void Set(uint32_t binding, Texture* texture) const override;
		virtual void Set(uint32_t binding, Texture* texture, uint32_t slot) const override;
	private:
		std::string ReadFile(const std::filesystem::path& filepath) const;
		
		void CreateDescriptorSetLayout(VkDevice device, const std::vector<ShaderDescriptorData>& shaderDescriptors);
		void CreateDescriptorPool(VkDevice device, const std::vector<ShaderDescriptorData>& shaderDescriptors);
		void CreateDescriptorSets(VkDevice device, const std::vector<ShaderDescriptorData>& shaderDescriptors);

		VkShaderModule CreateShaderModule(const std::vector<uint32_t>& shaderCode);
		std::vector<VkPipelineShaderStageCreateInfo> CreatePipelineShaderStageCreateInfos() const;
	private:
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		VkShaderModule m_VertexShaderModule = VK_NULL_HANDLE;
		VkShaderModule m_FragmentShaderModule = VK_NULL_HANDLE;

		std::vector<VkPipelineShaderStageCreateInfo> m_PipelineShaderStageCreateInfos;

		friend class VulkanPipeline;
	};

}
