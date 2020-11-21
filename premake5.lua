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
includeDir["glm"] = "Engine/vendor/glm"
includeDir["stb_image"] = "Engine/vendor/stb_image"
includeDir["vendor"] = "Engine/vendor"
--we include the GLFW folder, which basically means the lua-premake-file in this directory gets included
include "Engine/vendor/GLFW"
include "Engine/vendor/Glad"
include "Engine/vendor/imgui"

project "Engine"
	location "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "Engpch.hpp"
	pchsource "Engine/src/Engpch.cpp"
		
	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stb_image/**.hpp",
		"%{prj.name}/vendor/stb_image/**.cpp"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{includeDir.GLFW}",
		"%{includeDir.Glad}",
		"%{includeDir.ImGui}",
		"%{includeDir.glm}",
		"%{includeDir.stb_image}"
	}
	
	links
	{
		"GLFW",--we link the created project to the engine-project
		"Glad",
		"opengl32.lib",
		"ImGui"
	}
	
	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ENG_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",
			"KEYCODES_GLFW"
		}

	filter "configurations:Debug"
		defines "ENG_DEBUG"
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
	cppdialect "C++17"
	staticruntime "on"

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
		"%{includeDir.glm}",
		"%{includeDir.Glad}",
		"Engine/vendor",
		"Engine/src"
	}

	links
	{
		"Engine"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"ENG_PLATFORM_WINDOWS",
			"KEYCODES_GLFW"
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
