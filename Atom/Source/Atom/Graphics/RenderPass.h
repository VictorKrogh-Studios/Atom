#pragma once
#include "Atom/Graphics/Enumerations/ImageFormat.h"

#include <glm/glm.hpp>

namespace Atom
{

	struct RenderPassCreateInfo
	{
		Enumerations::ImageFormat ImageFormat;
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		glm::u32vec2 RenderArea;
	};

	class RenderPass
	{
	public:
		static RenderPass* Create(const RenderPassCreateInfo& renderPassCreateInfo);
	public:
		RenderPass(const RenderPassCreateInfo& renderPassCreateInfo);
		virtual ~RenderPass() = default;
	protected:
		RenderPassCreateInfo m_CreateInfo;
	};

}
