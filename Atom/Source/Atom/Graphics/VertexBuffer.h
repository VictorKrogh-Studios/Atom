#pragma once
#include "Atom/Graphics/Enumerations/BufferUsageFlags.h"

namespace Atom
{

	struct VertexBufferCreateInfo
	{
		Enumerations::BufferUsageFlags Usage;
		uint32_t Size;
		void* Vertices = nullptr;
	};

	class VertexBuffer
	{
	public:
		static VertexBuffer* Create(const VertexBufferCreateInfo& createInfo);
		static VertexBuffer* Create(uint64_t size, void* vertices);
	public:
		virtual ~VertexBuffer() = default;

		virtual void Upload(uint32_t size, void* vertices) const = 0;
	};

}
