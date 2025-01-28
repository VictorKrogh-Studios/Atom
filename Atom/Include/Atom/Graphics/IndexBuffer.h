#pragma once

namespace Atom
{

	class IndexBuffer
	{
	public:
		static IndexBuffer* Create(uint32_t count, uint32_t* indices);
	public:
		IndexBuffer(uint32_t count);
		virtual ~IndexBuffer() = default;

		uint32_t GetIndexCount() const { return m_Count; }
	protected:
		uint32_t m_Count;
	};

	typedef IndexBuffer ElementBuffer;

}
