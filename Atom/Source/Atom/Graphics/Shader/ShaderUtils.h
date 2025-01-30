#pragma once

#include <shaderc/shaderc.hpp>

namespace Atom::shaderc_utils
{

	inline static shaderc_shader_kind ShaderKindFromString(std::string& type)
	{
		const std::unordered_map<std::string, shaderc_shader_kind> map =
		{
			{ "vertex", shaderc_vertex_shader },
			{ "fragment", shaderc_fragment_shader },
			{ "pixel", shaderc_fragment_shader },
		};

		std::transform(type.begin(), type.end(), type.begin(), [](unsigned char c)
		{
			return std::tolower(c);
		});

		if (map.find(type) == map.end())
		{
			return shaderc_anyhit_shader;
		}

		return map.at(type);
	}

}
