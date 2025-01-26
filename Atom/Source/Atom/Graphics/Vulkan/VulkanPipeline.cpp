#include "ATPCH.h"
#include "VulkanPipeline.h"
#include "VulkanGraphicsContext.h"
#include "VulkanShader.h"
#include "VulkanUtils.h"
#include "VulkanRenderPass.h"
#include "VulkanUniformBuffer.h"
#include "VulkanStorageBuffer.h"

namespace Atom
{

	VulkanPipeline::VulkanPipeline(const PipelineOptions& options)
		: Pipeline(options)
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		CreateDescriptorSetLayout(device);
		CreateDescriptorPool(device);
		CreateDescriptorSet(device);

		CreatePipelineLayout(device);
		CreateGraphicsPipeline(device);
	}

	VulkanPipeline::~VulkanPipeline()
	{
		VkDevice device = VulkanGraphicsContext::GetDevice()->m_Device;

		vkDestroyDescriptorSetLayout(device, m_DescriptorSetLayout, nullptr);
		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);

		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
		vkDestroyPipeline(device, m_GraphicsPipeline, nullptr);
	}

	void VulkanPipeline::CreateDescriptorSetLayout(VkDevice device)
	{
		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		VkDescriptorSetLayoutBinding sboLayoutBinding{};
		sboLayoutBinding.binding = 1;
		sboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		sboLayoutBinding.descriptorCount = 1;
		sboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		sboLayoutBinding.pImmutableSamplers = nullptr; // Optional

		std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, sboLayoutBinding };

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &m_DescriptorSetLayout);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Descriptor Set Layout");
	}

	void VulkanPipeline::CreateDescriptorPool(VkDevice device)
	{
		std::vector<VkDescriptorPoolSize> poolSizes = {
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
		};

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolCreateInfo.pPoolSizes = poolSizes.data();
		descriptorPoolCreateInfo.maxSets = 3; // 3 = Frames in Flight

		VkResult result = vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Descriptor Pool");
	}

	void VulkanPipeline::CreateDescriptorSet(VkDevice device)
	{
		std::vector<VkDescriptorSetLayout> layouts(3, m_DescriptorSetLayout);

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(3); // 3 = Frames in Flight
		allocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(3); // 3 = Frames in Flight
		VkResult result = vkAllocateDescriptorSets(device, &allocInfo, m_DescriptorSets.data());
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate Descriptor Sets");

		VulkanUniformBuffer* uniformBuffer = static_cast<VulkanUniformBuffer*>(m_Options.UniformBuffer);

		for (size_t i = 0; i < 3; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = uniformBuffer->m_Buffers[i];
			bufferInfo.offset = 0;
			bufferInfo.range = uniformBuffer->GetSize();

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			descriptorWrite.pImageInfo = nullptr; // Optional
			descriptorWrite.pTexelBufferView = nullptr; // Optional

			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		}

		if (m_Options.StorageBuffer)
		{
			VulkanStorageBuffer* storageBuffer = static_cast<VulkanStorageBuffer*>(m_Options.StorageBuffer);

			for (size_t i = 0; i < 3; i++)
			{
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = storageBuffer->m_Buffers[i];
				bufferInfo.offset = 0;
				bufferInfo.range = storageBuffer->GetSize();

				VkWriteDescriptorSet descriptorWrite{};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = m_DescriptorSets[i];
				descriptorWrite.dstBinding = 1;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				descriptorWrite.descriptorCount = 1;
				descriptorWrite.pBufferInfo = &bufferInfo;
				descriptorWrite.pImageInfo = nullptr; // Optional
				descriptorWrite.pTexelBufferView = nullptr; // Optional

				vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
			}
		}
	}

	void VulkanPipeline::CreatePipelineLayout(VkDevice device)
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1; // Optional
		pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create pipeline layout");
	}

	void VulkanPipeline::CreateGraphicsPipeline(VkDevice device)
	{
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = m_Options.Layout.GetStride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
		vertexInputAttributeDescriptions.resize(m_Options.Layout.GetElements().size());

		uint32_t index = 0;
		for (auto& element : m_Options.Layout.GetElements())
		{
			vertexInputAttributeDescriptions[index].binding = bindingDescription.binding;
			vertexInputAttributeDescriptions[index].location = index;
			vertexInputAttributeDescriptions[index].format = Vulkan::Utils::GetVkFormatByShaderDataType(element.Type);
			vertexInputAttributeDescriptions[index].offset = element.Offset;

			index++;
		}

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data(); // Optional

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		//VkViewport viewport{};
		//viewport.x = 0.0f;
		//viewport.y = 0.0f;
		//viewport.width = (float)swapChainExtent.width;
		//viewport.height = (float)swapChainExtent.height;
		//viewport.minDepth = 0.0f;
		//viewport.maxDepth = 1.0f;

		//VkRect2D scissor{};
		//scissor.offset = { 0, 0 };
		//scissor.extent = swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_NONE; // TODO: REMEMBER ME!!!
		//rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		//rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
		/*
			if (blendEnable) {
				finalColor.rgb = (srcColorBlendFactor * newColor.rgb) <colorBlendOp> (dstColorBlendFactor * oldColor.rgb);
				finalColor.a = (srcAlphaBlendFactor * newColor.a) <alphaBlendOp> (dstAlphaBlendFactor * oldColor.a);
			} else {
				finalColor = newColor;
			}

			finalColor = finalColor & colorWriteMask;
		*/

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		VulkanShader* vulkanShader = static_cast<VulkanShader*>(m_Options.Shader);

		VkRenderPass renderPass = VK_NULL_HANDLE;
#if 0 
		renderPass = VulkanSwapChain::Get()->m_RenderPass;
#else
		renderPass = static_cast<VulkanRenderPass*>(m_Options.RenderPass)->m_RenderPass;
#endif

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = static_cast<uint32_t>(vulkanShader->m_PipelineShaderStageCreateInfos.size());
		pipelineInfo.pStages = vulkanShader->m_PipelineShaderStageCreateInfos.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline);
		AT_CORE_ASSERT(result == VK_SUCCESS, "Failed to create graphics pipeline");
	}

}