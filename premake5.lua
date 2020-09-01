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

project "Engine"
	location "Engine"
	kind "SharedLib"
	language "C++"

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
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"ENG_PLATFORM_WINDOWS",
			"ENG_BUILD_DLL",
			"_WINDLL"
		}

		postbuildcommands
		{
			"{COPY} %{cfg.buildtarget.relpath} ../bin/" ..outputdir .. "/Sandbox"
		}

	filter "configurations:Debug"
		defines "ENG_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "ENG_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "ENG_DIST"
		optimize "On"
		
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

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
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"ENG_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "ENG_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "ENG_RELEASE"
		optimize "on"

	filter "configurations:Dist"
		defines "ENG_DIST"
		optimize "on"
