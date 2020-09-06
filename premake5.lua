workspace "Engine"
	architecture "x64"
	startproject "Sandbox"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

--we make a luaTable(this will grow) and put in "GLFW" as an element
includeDir = {}
includeDir["GLFW"] = "Engine/vendor/GLFW/include"
includeDir["Glad"] = "Engine/vendor/Glad/include"
includeDir["ImGui"] = "Engine/vendor/imgui"
--we include the GLFW folder, which basically means the lua-premake-file in this directory gets included
include "Engine/vendor/GLFW"
include "Engine/vendor/Glad"
include "Engine/vendor/imgui"

project "Engine"
	location "Engine"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "Engpch.hpp"
	pchsource "Engine/src/Engpch.cpp"
		
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{includeDir.GLFW}",
		"%{includeDir.Glad}",
		"%{includeDir.ImGui}"
	}
	
	links
	{
		"GLFW",--we link the created project to the engine-project
		"Glad",
		"opengl32.lib",
		"ImGui"
	}
	
	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"ENG_PLATFORM_WINDOWS",
			"ENG_BUILD_DLL",
			"_WINDLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			"{COPY} %{cfg.buildtarget.relpath} ../bin/" ..outputdir .. "/Sandbox"
		}

	filter "configurations:Debug"
		defines {"ENG_DEBUG", "ENG_ENABLE_ASSERTS"}
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "ENG_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "ENG_DIST"
		runtime "Release"
		optimize "On"
		
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Engine/vendor/spdlog/include",
		"Engine/src"
	}

	links
	{
		"Engine"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"ENG_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ENG_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENG_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ENG_DIST"
		runtime "Release"
		optimize "on"
