project "Atom"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"
	
	pchheader "ATPCH.h"
	pchsource "Source/ATPCH.cpp"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"Source/**.h",
		"Source/**.inl",
		"Source/**.cpp",
		"Include/**.h",
		"Include/**.inl",
        "Vendor/glm/glm/**.hpp",
		"Vendor/glm/glm/**.inl",
		"Vendor/stb_image/**.h",
		"Vendor/stb_image/**.cpp"
	}
    
    includedirs {
		"Source",
		"Include",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glfw}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.vulkan}",
        "%{IncludeDir.stb_image}"
    }
	
	links {
		"spdlog",
		"glfw",
		"imgui",
		"%{Library.vulkan}"
    }

	defines {
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

    buildoptions {
        "/utf-8"
    }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "AT_DEBUG"
		runtime "Debug"
		symbols "on"

		links {
			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}"
		}

	filter "configurations:Release"
		defines "AT_RELEASE"
		runtime "Release"
		optimize "on"

		links {
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}