#pragma once

namespace Atom
{

	class StorageBuffer
	{
	public:
		static StorageBuffer* Create(uint64_t size);
	public:
		StorageBuffer(uint64_t size);
		virtual ~StorageBuffer() = default;

		virtual void Upload(uint64_t size, void* data, uint32_t index) const = 0;

		virtual uint64_t GetSize() const { return m_Size; }
	protected:
		uint64_t m_Size;
	};

}
