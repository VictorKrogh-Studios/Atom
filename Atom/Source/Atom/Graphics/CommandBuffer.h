#pragma once

namespace Atom
{

	class CommandBuffer
	{
	public:
		static CommandBuffer* Create();
	public:
		virtual ~CommandBuffer() = default;
	};

}
