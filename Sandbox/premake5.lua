project "Sandbox"
	kind "ConsoleApp"
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
		"%{wks.location}/Atom/Source",
        "%{IncludeDir.spdlog}"
    }

	links {
		"Atom",
	}

    buildoptions {
        "/utf-8"
    }

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"