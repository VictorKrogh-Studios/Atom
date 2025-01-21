#pragma once
#include "Atom/Graphics/Enumerations/ImageFormat.h"
#include "Atom/Graphics/Enumerations/RenderPassSubpassContents.h"
#include "Atom/Graphics/Enumerations/RenderPassAttachmentLoadOperation.h"

#include <glm/glm.hpp>

namespace Atom
{

	struct RenderPassCreateInfo
	{
		Enumerations::ImageFormat ImageFormat;
		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		glm::u32vec2 RenderArea;
		Enumerations::RenderPassSubpassContents SubpassContents = Enumerations::RenderPassSubpassContents::Inline;
		Enumerations::RenderPassAttachmentLoadOperation LoadOperation = Enumerations::RenderPassAttachmentLoadOperation::Clear;
		bool TargetSwapChain = false;
		bool ImplicitSetViewport = true;
		bool ImplicitSetScissor = true;
	};

	class RenderPass
	{
	public:
		static RenderPass* Create(const RenderPassCreateInfo& renderPassCreateInfo);
	public:
		RenderPass(const RenderPassCreateInfo& renderPassCreateInfo);
		virtual ~RenderPass() = default;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
	protected:
		RenderPassCreateInfo m_CreateInfo;
	};

}
