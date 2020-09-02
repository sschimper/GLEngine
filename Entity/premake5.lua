include "../premakeDefines.lua"

project "Entity"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	SetupProject("Entity")
	
	PrecompiledHeaders("Entity")
	
	Link("Utils")
	Link("Core")

	includedirs
	{
		"../%{IncludeDir.GLM}",
		"../Renderer",
		"../GLRenderer",
		"../Physics",
		"../%{IncludeDir.pugixml}",
		"../%{IncludeDir.fmt}",
		"../%{IncludeDir.ImGui}",
	}

	links{
		"pugixml",
	}

	defines
	{
		"MODULE_CTX=Entity"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines
		{
			"BUILD_ENTITY_DLL",
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"