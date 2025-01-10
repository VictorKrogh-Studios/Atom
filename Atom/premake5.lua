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
		"Source/**.cpp"
	}
    
    includedirs {
		"Source",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.glfw}"
    }

	links {
        "spdlog",
		"glfw"
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

	filter "configurations:Release"
		defines "AT_RELEASE"
		runtime "Release"
		optimize "on"