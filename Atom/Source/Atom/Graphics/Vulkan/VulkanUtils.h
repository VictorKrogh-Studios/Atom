#pragma once
#include "Atom/Graphics/Enumerations/ImageFormat.h"
#include "Atom/Graphics/Enumerations/ShaderDataType.h"
#include "Atom/Graphics/Enumerations/CommandBufferLevel.h"
#include "Atom/Graphics/Enumerations/CommandBufferUsageFlags.h"
#include "Atom/Graphics/Enumerations/RenderPassAttachmentLoadOperation.h"
#include "Atom/Graphics/Enumerations/BufferUsageFlags.h"

#include <vulkan/vulkan.h>

namespace Atom::Vulkan::Utils
{

	inline static VkBufferUsageFlags GetVkBufferUsageFlags(Enumerations::BufferUsageFlags bufferUsageFlags)
	{
		switch (bufferUsageFlags)
		{
			case Atom::Enumerations::BufferUsageFlags::VertexBuffer: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			case Atom::Enumerations::BufferUsageFlags::IndexBuffer: return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			case Atom::Enumerations::BufferUsageFlags::None:
			default: break;
		}

		AT_CORE_ASSERT(true, "Invalid buffer usage flag");
		return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
	}

	inline static VkAttachmentLoadOp GetVkAttachmentLoadOp(Enumerations::RenderPassAttachmentLoadOperation renderPassLoadOperation)
	{
		switch (renderPassLoadOperation)
		{
			case Atom::Enumerations::RenderPassAttachmentLoadOperation::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
			case Atom::Enumerations::RenderPassAttachmentLoadOperation::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
			case Atom::Enumerations::RenderPassAttachmentLoadOperation::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			default: break;
		}

		AT_CORE_ASSERT(true, "Invalid render pass attachment load operation");
		return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
	}

	inline static VkCommandBufferUsageFlags GetVkCommandBufferUsageFlags(Enumerations::CommandBufferUsageFlags commandBufferUsageFlags)
	{
		switch (commandBufferUsageFlags)
		{
			case Atom::Enumerations::CommandBufferUsageFlags::ONE_TIME_SUBMIT_BIT: return VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			case Atom::Enumerations::CommandBufferUsageFlags::RENDER_PASS_CONTINUE_BIT: return VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
			case Atom::Enumerations::CommandBufferUsageFlags::SIMULTANEOUS_USE_BIT: return VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			case Atom::Enumerations::CommandBufferUsageFlags::None:
			default: break;
		}

		AT_CORE_ASSERT(true, "Unknown command buffer usage flag");
		return VK_COMMAND_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
	}

	inline static VkCommandBufferLevel GetVkCommandBufferLevel(Enumerations::CommandBufferLevel commandBufferLevel)
	{
		switch (commandBufferLevel)
		{
			case Atom::Enumerations::CommandBufferLevel::Primary: return VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			case Atom::Enumerations::CommandBufferLevel::Secondary:	return VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			default: break;
		}

		AT_CORE_ASSERT(true, "Unknown command buffer level");
		return VK_COMMAND_BUFFER_LEVEL_MAX_ENUM;
	}

	inline static VkFormat GetVkFormat(Enumerations::ImageFormat imageFormat)
	{
		switch (imageFormat)
		{
			case Atom::Enumerations::ImageFormat::B8G8R8A8_UNORM: return VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
			case Atom::Enumerations::ImageFormat::Unknown:
			default: break;
		}

		AT_CORE_ASSERT(true, "Unknown image format");
		return VkFormat::VK_FORMAT_UNDEFINED;
	}

	inline static Enumerations::ImageFormat GetImageFormat(VkFormat vkFormat)
	{
		switch (vkFormat)
		{
			case VK_FORMAT_UNDEFINED:
				break;
			case VK_FORMAT_R4G4_UNORM_PACK8:
				break;
			case VK_FORMAT_R4G4B4A4_UNORM_PACK16:
				break;
			case VK_FORMAT_B4G4R4A4_UNORM_PACK16:
				break;
			case VK_FORMAT_R5G6B5_UNORM_PACK16:
				break;
			case VK_FORMAT_B5G6R5_UNORM_PACK16:
				break;
			case VK_FORMAT_R5G5B5A1_UNORM_PACK16:
				break;
			case VK_FORMAT_B5G5R5A1_UNORM_PACK16:
				break;
			case VK_FORMAT_A1R5G5B5_UNORM_PACK16:
				break;
			case VK_FORMAT_R8_UNORM:
				break;
			case VK_FORMAT_R8_SNORM:
				break;
			case VK_FORMAT_R8_USCALED:
				break;
			case VK_FORMAT_R8_SSCALED:
				break;
			case VK_FORMAT_R8_UINT:
				break;
			case VK_FORMAT_R8_SINT:
				break;
			case VK_FORMAT_R8_SRGB:
				break;
			case VK_FORMAT_R8G8_UNORM:
				break;
			case VK_FORMAT_R8G8_SNORM:
				break;
			case VK_FORMAT_R8G8_USCALED:
				break;
			case VK_FORMAT_R8G8_SSCALED:
				break;
			case VK_FORMAT_R8G8_UINT:
				break;
			case VK_FORMAT_R8G8_SINT:
				break;
			case VK_FORMAT_R8G8_SRGB:
				break;
			case VK_FORMAT_R8G8B8_UNORM:
				break;
			case VK_FORMAT_R8G8B8_SNORM:
				break;
			case VK_FORMAT_R8G8B8_USCALED:
				break;
			case VK_FORMAT_R8G8B8_SSCALED:
				break;
			case VK_FORMAT_R8G8B8_UINT:
				break;
			case VK_FORMAT_R8G8B8_SINT:
				break;
			case VK_FORMAT_R8G8B8_SRGB:
				break;
			case VK_FORMAT_B8G8R8_UNORM:
				break;
			case VK_FORMAT_B8G8R8_SNORM:
				break;
			case VK_FORMAT_B8G8R8_USCALED:
				break;
			case VK_FORMAT_B8G8R8_SSCALED:
				break;
			case VK_FORMAT_B8G8R8_UINT:
				break;
			case VK_FORMAT_B8G8R8_SINT:
				break;
			case VK_FORMAT_B8G8R8_SRGB:
				break;
			case VK_FORMAT_R8G8B8A8_UNORM:
				break;
			case VK_FORMAT_R8G8B8A8_SNORM:
				break;
			case VK_FORMAT_R8G8B8A8_USCALED:
				break;
			case VK_FORMAT_R8G8B8A8_SSCALED:
				break;
			case VK_FORMAT_R8G8B8A8_UINT:
				break;
			case VK_FORMAT_R8G8B8A8_SINT:
				break;
			case VK_FORMAT_R8G8B8A8_SRGB:
				break;
			case VK_FORMAT_B8G8R8A8_UNORM: return Enumerations::ImageFormat::B8G8R8A8_UNORM;
			case VK_FORMAT_B8G8R8A8_SNORM:
				break;
			case VK_FORMAT_B8G8R8A8_USCALED:
				break;
			case VK_FORMAT_B8G8R8A8_SSCALED:
				break;
			case VK_FORMAT_B8G8R8A8_UINT:
				break;
			case VK_FORMAT_B8G8R8A8_SINT:
				break;
			case VK_FORMAT_B8G8R8A8_SRGB:
				break;
			case VK_FORMAT_A8B8G8R8_UNORM_PACK32:
				break;
			case VK_FORMAT_A8B8G8R8_SNORM_PACK32:
				break;
			case VK_FORMAT_A8B8G8R8_USCALED_PACK32:
				break;
			case VK_FORMAT_A8B8G8R8_SSCALED_PACK32:
				break;
			case VK_FORMAT_A8B8G8R8_UINT_PACK32:
				break;
			case VK_FORMAT_A8B8G8R8_SINT_PACK32:
				break;
			case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
				break;
			case VK_FORMAT_A2R10G10B10_UNORM_PACK32:
				break;
			case VK_FORMAT_A2R10G10B10_SNORM_PACK32:
				break;
			case VK_FORMAT_A2R10G10B10_USCALED_PACK32:
				break;
			case VK_FORMAT_A2R10G10B10_SSCALED_PACK32:
				break;
			case VK_FORMAT_A2R10G10B10_UINT_PACK32:
				break;
			case VK_FORMAT_A2R10G10B10_SINT_PACK32:
				break;
			case VK_FORMAT_A2B10G10R10_UNORM_PACK32:
				break;
			case VK_FORMAT_A2B10G10R10_SNORM_PACK32:
				break;
			case VK_FORMAT_A2B10G10R10_USCALED_PACK32:
				break;
			case VK_FORMAT_A2B10G10R10_SSCALED_PACK32:
				break;
			case VK_FORMAT_A2B10G10R10_UINT_PACK32:
				break;
			case VK_FORMAT_A2B10G10R10_SINT_PACK32:
				break;
			case VK_FORMAT_R16_UNORM:
				break;
			case VK_FORMAT_R16_SNORM:
				break;
			case VK_FORMAT_R16_USCALED:
				break;
			case VK_FORMAT_R16_SSCALED:
				break;
			case VK_FORMAT_R16_UINT:
				break;
			case VK_FORMAT_R16_SINT:
				break;
			case VK_FORMAT_R16_SFLOAT:
				break;
			case VK_FORMAT_R16G16_UNORM:
				break;
			case VK_FORMAT_R16G16_SNORM:
				break;
			case VK_FORMAT_R16G16_USCALED:
				break;
			case VK_FORMAT_R16G16_SSCALED:
				break;
			case VK_FORMAT_R16G16_UINT:
				break;
			case VK_FORMAT_R16G16_SINT:
				break;
			case VK_FORMAT_R16G16_SFLOAT:
				break;
			case VK_FORMAT_R16G16B16_UNORM:
				break;
			case VK_FORMAT_R16G16B16_SNORM:
				break;
			case VK_FORMAT_R16G16B16_USCALED:
				break;
			case VK_FORMAT_R16G16B16_SSCALED:
				break;
			case VK_FORMAT_R16G16B16_UINT:
				break;
			case VK_FORMAT_R16G16B16_SINT:
				break;
			case VK_FORMAT_R16G16B16_SFLOAT:
				break;
			case VK_FORMAT_R16G16B16A16_UNORM:
				break;
			case VK_FORMAT_R16G16B16A16_SNORM:
				break;
			case VK_FORMAT_R16G16B16A16_USCALED:
				break;
			case VK_FORMAT_R16G16B16A16_SSCALED:
				break;
			case VK_FORMAT_R16G16B16A16_UINT:
				break;
			case VK_FORMAT_R16G16B16A16_SINT:
				break;
			case VK_FORMAT_R16G16B16A16_SFLOAT:
				break;
			case VK_FORMAT_R32_UINT:
				break;
			case VK_FORMAT_R32_SINT:
				break;
			case VK_FORMAT_R32_SFLOAT:
				break;
			case VK_FORMAT_R32G32_UINT:
				break;
			case VK_FORMAT_R32G32_SINT:
				break;
			case VK_FORMAT_R32G32_SFLOAT:
				break;
			case VK_FORMAT_R32G32B32_UINT:
				break;
			case VK_FORMAT_R32G32B32_SINT:
				break;
			case VK_FORMAT_R32G32B32_SFLOAT:
				break;
			case VK_FORMAT_R32G32B32A32_UINT:
				break;
			case VK_FORMAT_R32G32B32A32_SINT:
				break;
			case VK_FORMAT_R32G32B32A32_SFLOAT:
				break;
			case VK_FORMAT_R64_UINT:
				break;
			case VK_FORMAT_R64_SINT:
				break;
			case VK_FORMAT_R64_SFLOAT:
				break;
			case VK_FORMAT_R64G64_UINT:
				break;
			case VK_FORMAT_R64G64_SINT:
				break;
			case VK_FORMAT_R64G64_SFLOAT:
				break;
			case VK_FORMAT_R64G64B64_UINT:
				break;
			case VK_FORMAT_R64G64B64_SINT:
				break;
			case VK_FORMAT_R64G64B64_SFLOAT:
				break;
			case VK_FORMAT_R64G64B64A64_UINT:
				break;
			case VK_FORMAT_R64G64B64A64_SINT:
				break;
			case VK_FORMAT_R64G64B64A64_SFLOAT:
				break;
			case VK_FORMAT_B10G11R11_UFLOAT_PACK32:
				break;
			case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
				break;
			case VK_FORMAT_D16_UNORM:
				break;
			case VK_FORMAT_X8_D24_UNORM_PACK32:
				break;
			case VK_FORMAT_D32_SFLOAT:
				break;
			case VK_FORMAT_S8_UINT:
				break;
			case VK_FORMAT_D16_UNORM_S8_UINT:
				break;
			case VK_FORMAT_D24_UNORM_S8_UINT:
				break;
			case VK_FORMAT_D32_SFLOAT_S8_UINT:
				break;
			case VK_FORMAT_BC1_RGB_UNORM_BLOCK:
				break;
			case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
				break;
			case VK_FORMAT_BC1_RGBA_UNORM_BLOCK:
				break;
			case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
				break;
			case VK_FORMAT_BC2_UNORM_BLOCK:
				break;
			case VK_FORMAT_BC2_SRGB_BLOCK:
				break;
			case VK_FORMAT_BC3_UNORM_BLOCK:
				break;
			case VK_FORMAT_BC3_SRGB_BLOCK:
				break;
			case VK_FORMAT_BC4_UNORM_BLOCK:
				break;
			case VK_FORMAT_BC4_SNORM_BLOCK:
				break;
			case VK_FORMAT_BC5_UNORM_BLOCK:
				break;
			case VK_FORMAT_BC5_SNORM_BLOCK:
				break;
			case VK_FORMAT_BC6H_UFLOAT_BLOCK:
				break;
			case VK_FORMAT_BC6H_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_BC7_UNORM_BLOCK:
				break;
			case VK_FORMAT_BC7_SRGB_BLOCK:
				break;
			case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK:
				break;
			case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
				break;
			case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK:
				break;
			case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
				break;
			case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK:
				break;
			case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
				break;
			case VK_FORMAT_EAC_R11_UNORM_BLOCK:
				break;
			case VK_FORMAT_EAC_R11_SNORM_BLOCK:
				break;
			case VK_FORMAT_EAC_R11G11_UNORM_BLOCK:
				break;
			case VK_FORMAT_EAC_R11G11_SNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
				break;
			case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
				break;
			case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
				break;
			case VK_FORMAT_G8B8G8R8_422_UNORM:
				break;
			case VK_FORMAT_B8G8R8G8_422_UNORM:
				break;
			case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM:
				break;
			case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM:
				break;
			case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM:
				break;
			case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM:
				break;
			case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM:
				break;
			case VK_FORMAT_R10X6_UNORM_PACK16:
				break;
			case VK_FORMAT_R10X6G10X6_UNORM_2PACK16:
				break;
			case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16:
				break;
			case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16:
				break;
			case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16:
				break;
			case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16:
				break;
			case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16:
				break;
			case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16:
				break;
			case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16:
				break;
			case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16:
				break;
			case VK_FORMAT_R12X4_UNORM_PACK16:
				break;
			case VK_FORMAT_R12X4G12X4_UNORM_2PACK16:
				break;
			case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16:
				break;
			case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16:
				break;
			case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16:
				break;
			case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16:
				break;
			case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16:
				break;
			case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16:
				break;
			case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16:
				break;
			case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16:
				break;
			case VK_FORMAT_G16B16G16R16_422_UNORM:
				break;
			case VK_FORMAT_B16G16R16G16_422_UNORM:
				break;
			case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM:
				break;
			case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM:
				break;
			case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM:
				break;
			case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM:
				break;
			case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM:
				break;
			case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM:
				break;
			case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16:
				break;
			case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16:
				break;
			case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM:
				break;
			case VK_FORMAT_A4R4G4B4_UNORM_PACK16:
				break;
			case VK_FORMAT_A4B4G4R4_UNORM_PACK16:
				break;
			case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK:
				break;
			case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG:
				break;
			case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG:
				break;
			case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG:
				break;
			case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG:
				break;
			case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
				break;
			case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
				break;
			case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
				break;
			case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
				break;
			case VK_FORMAT_R16G16_SFIXED5_NV:
				break;
			case VK_FORMAT_A1B5G5R5_UNORM_PACK16_KHR:
				break;
			case VK_FORMAT_A8_UNORM_KHR:
				break;
			case VK_FORMAT_MAX_ENUM:
				break;
			default:
				break;
		}

		AT_CORE_ASSERT(true, "Unknown image format");
		return Enumerations::ImageFormat::Unknown;
	}

	inline static VkFormat GetVkFormatByShaderDataType(Enumerations::ShaderDataType shaderDataType)
	{
		switch (shaderDataType)
		{
			case Atom::Enumerations::ShaderDataType::Float: return VkFormat::VK_FORMAT_R32_SFLOAT;
			case Atom::Enumerations::ShaderDataType::Float2: return VkFormat::VK_FORMAT_R32G32_SFLOAT;
			case Atom::Enumerations::ShaderDataType::Float3: return VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
			case Atom::Enumerations::ShaderDataType::Float4: return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
			case Atom::Enumerations::ShaderDataType::None:
			default: break;
		}

		AT_CORE_ASSERT(true, "Unknown image format");
		return VkFormat::VK_FORMAT_UNDEFINED;
	}

}
