#pragma once
#include "Atom/Graphics/Shader.h"

namespace Atom
{

	class ShaderLibrary
	{
	public:
		ShaderLibrary() = default;
		~ShaderLibrary();

		void Add(Shader* shader);
		void Add(const std::string& name, Shader* shader);

		void Load(const std::filesystem::path& filepath);
		void Load(const std::string& name, const std::filesystem::path& filepath);

		Shader* Get(const std::string& name) const;

		std::unordered_map<std::string, Shader*>& GetShaders() { return m_Shaders; }
		const std::unordered_map<std::string, Shader*>& GetShaders() const { return m_Shaders; }
	private:
		std::string GetFileName(const std::filesystem::path& filepath) const;
	private:
		std::unordered_map<std::string, Shader*> m_Shaders;
	};

}
