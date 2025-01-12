#pragma once

namespace Atom
{

	class Shader
	{
	public:
		static Shader* CreateFromFile(const std::string& vertexFilePath, const std::string& fragmentFilePath);
	public:
		virtual ~Shader() = default;
	};

}
