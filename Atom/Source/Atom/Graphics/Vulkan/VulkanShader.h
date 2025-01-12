#pragma once
#include "Atom/Graphics/Shader.h"

namespace Atom
{

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::string& vertexFilePath, const std::string& fragmentFilePath);
		~VulkanShader();
	};

}
