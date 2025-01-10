project "Atom"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"Source/**.h",
		"Source/**.cpp"
	}
    
    includedirs {
		"Source"
    }

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "AT_GFX_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "AT_GFX_RELEASE"
		runtime "Release"
		optimize "on"