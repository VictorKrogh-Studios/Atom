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

IncludeDir = {}
IncludeDir['spdlog'] = "%{wks.location}/Atom/Vendor/spdlog/include";
IncludeDir['glfw'] = "%{wks.location}/Atom/Vendor/glfw/include";

group "Dependencies"
    include "Atom/Vendor/spdlog"
    include "Atom/Vendor/glfw"
group ""

include "Atom"
include "Sandbox"