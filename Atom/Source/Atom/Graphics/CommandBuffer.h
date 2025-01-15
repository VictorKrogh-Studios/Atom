#pragma once

namespace Atom
{

	class CommandBuffer
	{
	public:
		static CommandBuffer* Create();
		static CommandBuffer* Create(uint32_t count);
	public:
		CommandBuffer(uint32_t count);
		virtual ~CommandBuffer() = default;
	protected:
		uint32_t m_Count;
	};

}
