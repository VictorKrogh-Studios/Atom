#pragma once

#include "Atom/Graphics/Enumerations/ShaderDataType.h"

namespace Atom
{

	static uint32_t ShaderDataTypeSize(Enumerations::ShaderDataType type)
	{
		switch (type)
		{
			case Enumerations::ShaderDataType::Float:    return 4;
			case Enumerations::ShaderDataType::Float2:   return 4 * 2;
			case Enumerations::ShaderDataType::Float3:   return 4 * 3;
			case Enumerations::ShaderDataType::Float4:   return 4 * 4;
			case Enumerations::ShaderDataType::Mat3:     return 4 * 3 * 3;
			case Enumerations::ShaderDataType::Mat4:     return 4 * 4 * 4;
			case Enumerations::ShaderDataType::Int:      return 4;
			case Enumerations::ShaderDataType::Int2:     return 4 * 2;
			case Enumerations::ShaderDataType::Int3:     return 4 * 3;
			case Enumerations::ShaderDataType::Int4:     return 4 * 4;
			case Enumerations::ShaderDataType::Bool:     return 1;
		}

		AT_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct VertexAttributeElement
	{
		std::string Name;
		Enumerations::ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;

		VertexAttributeElement() = default;

		VertexAttributeElement(Enumerations::ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case Enumerations::ShaderDataType::Float:   return 1;
				case Enumerations::ShaderDataType::Float2:  return 2;
				case Enumerations::ShaderDataType::Float3:  return 3;
				case Enumerations::ShaderDataType::Float4:  return 4;
				case Enumerations::ShaderDataType::Mat3:    return 3;
				case Enumerations::ShaderDataType::Mat4:    return 4;
				case Enumerations::ShaderDataType::Int:     return 1;
				case Enumerations::ShaderDataType::Int2:    return 2;
				case Enumerations::ShaderDataType::Int3:    return 3;
				case Enumerations::ShaderDataType::Int4:    return 4;
				case Enumerations::ShaderDataType::Bool:    return 1;
			}

			AT_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class VertexAttributeLayout
	{
	public:
		VertexAttributeLayout()
		{
		}

		VertexAttributeLayout(std::initializer_list<VertexAttributeElement> elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const
		{
			return m_Stride;
		}
		const std::vector<VertexAttributeElement>& GetElements() const
		{
			return m_Elements;
		}

		std::vector<VertexAttributeElement>::iterator begin()
		{
			return m_Elements.begin();
		}
		std::vector<VertexAttributeElement>::iterator end()
		{
			return m_Elements.end();
		}
		std::vector<VertexAttributeElement>::const_iterator begin() const
		{
			return m_Elements.begin();
		}
		std::vector<VertexAttributeElement>::const_iterator end() const
		{
			return m_Elements.end();
		}
	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<VertexAttributeElement> m_Elements;
		uint32_t m_Stride = 0;
	};

}