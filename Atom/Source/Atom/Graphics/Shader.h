#pragma once

namespace Atom
{

	class Shader
	{
	public:
		static Shader* CreateFromFile(const std::string& vertexFilePath, const std::string& fragmentFilePath);
	protected:
		virtual std::vector<char> ReadSpvFile(const std::string& filepath) const;
	public:
		virtual ~Shader() = default;
	};

}
