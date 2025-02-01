#pragma once
#include "Atom/Graphics/Enumerations/CommandBufferLevel.h"
#include "Atom/Graphics/Enumerations/CommandBufferUsageFlags.h"

namespace Atom
{

	class RenderPass;

	struct CommandBufferCreateInfo
	{
		Enumerations::CommandBufferLevel Level;
		Enumerations::CommandBufferUsageFlags Usage;
		bool TargetSwapChain = false;
	};

	class CommandBuffer
	{
	public:
		static CommandBuffer* Create(const CommandBufferCreateInfo& commandBufferCreateInfo);
		static CommandBuffer* Create(const CommandBufferCreateInfo& commandBufferCreateInfo, uint32_t count);
	public:
		CommandBuffer(const CommandBufferCreateInfo& commandBufferCreateInfo, uint32_t count);
		virtual ~CommandBuffer() = default;

		virtual void Reset(uint32_t index) const = 0;
		virtual void Begin(uint32_t index) const = 0;
		virtual void Begin(RenderPass* renderPass, uint32_t index) const = 0;
		virtual void End(uint32_t index) const = 0;
		virtual void Submit(uint32_t index) const = 0;
		virtual void Execute(CommandBuffer* commandBuffer, uint32_t index) const = 0;
	protected:
		CommandBufferCreateInfo m_CommandBufferCreateInfo;
		uint32_t m_Count;
	};

}
