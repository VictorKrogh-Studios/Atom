workspace "Atom"
	architecture "x86_64"
	startproject "Sandbox"

	configurations { 
		"Debug",
		"Release"
	}

    flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Grab Vulkan SDK path
VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir['spdlog'] = "%{wks.location}/Atom/Vendor/spdlog/include";
IncludeDir['glfw'] = "%{wks.location}/Atom/Vendor/glfw/include";
IncludeDir['vulkan'] = "%{VULKAN_SDK}/Include";

LibraryDir = {}
LibraryDir['vulkan'] = "%{VULKAN_SDK}/lib";

Library = {}
Library['vulkan'] = "%{LibraryDir.vulkan}/vulkan-1.lib";

group "Dependencies"
    include "Atom/Vendor/spdlog"
    include "Atom/Vendor/glfw"
group ""

include "Atom"
include "Sandbox"