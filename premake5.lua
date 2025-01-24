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
IncludeDir['glm'] = "%{wks.location}/Atom/Vendor/glm";
IncludeDir["ImGui"] = "%{wks.location}/Atom/Vendor/imgui";
IncludeDir['vulkan'] = "%{VULKAN_SDK}/Include";

LibraryDir = {}
LibraryDir['vulkan'] = "%{VULKAN_SDK}/lib";

Library = {}
Library['vulkan'] = "%{LibraryDir.vulkan}/vulkan-1.lib";
Library["ShaderC_Debug"] = "%{LibraryDir.vulkan}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.vulkan}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.vulkan}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.vulkan}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.vulkan}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.vulkan}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.vulkan}/spirv-cross-glsl.lib"

group "Dependencies"
    include "Atom/Vendor/spdlog"
    include "Atom/Vendor/glfw"
	include "Atom/Vendor/imgui"
group ""

include "Atom"
include "Sandbox"