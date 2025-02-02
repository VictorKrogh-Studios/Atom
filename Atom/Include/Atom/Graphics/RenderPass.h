#pragma once
#include "Atom/Graphics/Enumerations/ImageFormat.h"
#include "Atom/Graphics/Enumerations/RenderPassSubpassContents.h"
#include "Atom/Graphics/Enumerations/RenderPassAttachmentLoadOperation.h"
#include "Atom/Graphics/Enumerations/AttachmentLoadOperation.h"

#include <glm/glm.hpp>

namespace Atom
{

	struct RenderPassAttachment
	{
		RenderPassAttachment() = default;
		RenderPassAttachment(Enumerations::ImageFormat format) : Format(format)
		{
		}

		Enumerations::ImageFormat Format;
		Enumerations::AttachmentLoadOperation LoadOperation = Enumerations::AttachmentLoadOperation::Inherit;
	};

	struct RenderPassAttachments
	{
		RenderPassAttachments() = default;
		RenderPassAttachments(const std::initializer_list<RenderPassAttachment>& attachments) : Attachments(attachments)
		{
		}

		std::vector< RenderPassAttachment> Attachments;

		std::vector<RenderPassAttachment>::iterator begin()
		{
			return Attachments.begin();
		}
		std::vector<RenderPassAttachment>::iterator end()
		{
			return Attachments.end();
		}
		std::vector<RenderPassAttachment>::reverse_iterator rbegin()
		{
			return Attachments.rbegin();
		}
		std::vector<RenderPassAttachment>::reverse_iterator rend()
		{
			return Attachments.rend();
		}

		std::vector<RenderPassAttachment>::const_iterator begin() const
		{
			return Attachments.begin();
		}
		std::vector<RenderPassAttachment>::const_iterator end()	const
		{
			return Attachments.end();
		}
		std::vector<RenderPassAttachment>::const_reverse_iterator rbegin() const
		{
			return Attachments.rbegin();
		}
		std::vector<RenderPassAttachment>::const_reverse_iterator rend() const
		{
			return Attachments.rend();
		}
	};

	struct RenderPassCreateInfo
	{
		RenderPassAttachments Attachments;
		bool ClearColorOnLoad = true;
		bool ClearDepthOnLoad = true;

		glm::vec4 ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		glm::u32vec2 RenderArea;
		Enumerations::RenderPassSubpassContents SubpassContents = Enumerations::RenderPassSubpassContents::Inline;
		bool TargetSwapChain = false;
		bool ImplicitSetViewport = true;
		bool ImplicitSetScissor = true;
	};

	class Framebuffer;

	class RenderPass
	{
	public:
		static RenderPass* Create(const RenderPassCreateInfo& renderPassCreateInfo);
	public:
		RenderPass(const RenderPassCreateInfo& renderPassCreateInfo);
		virtual ~RenderPass() = default;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual void SetRenderTarget(Framebuffer* framebuffer) = 0;

		virtual const RenderPassAttachments& GetAttachments() const { return m_CreateInfo.Attachments; }
	protected:
		RenderPassCreateInfo m_CreateInfo;
	};

}
