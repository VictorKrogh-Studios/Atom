#pragma once

namespace Atom
{

	class UniformBuffer
	{
	public:
		static UniformBuffer* Create(uint64_t size);
	public:
		UniformBuffer(uint64_t size);
		virtual ~UniformBuffer() = default;

		virtual void Upload(void* data, uint64_t size, uint32_t frameIndex) const = 0;

		virtual uint64_t GetSize() const { return m_Size; }
	protected:
		uint64_t m_Size;
	};

}
