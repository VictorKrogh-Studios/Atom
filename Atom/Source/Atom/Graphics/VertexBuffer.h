#pragma once

namespace Atom
{

	class VertexBuffer
	{
	public:
		static VertexBuffer* Create(uint64_t size, void* vertices);
	public:
		virtual ~VertexBuffer() = default;
	};

}
