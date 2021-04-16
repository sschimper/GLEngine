require('vstudio')
premake.api.register {
  name = "workspace_files",
  scope = "workspace",
  kind = "list:string",
}

premake.override(premake.vstudio.sln2005, "projects", function(base, wks)
  if wks.workspace_files and #wks.workspace_files > 0 then
    premake.push('Project("{2150E333-8FDC-42A3-9474-1A3956D46DE8}") = "Solution Items", "Solution Items", "{' .. os.uuid("Solution Items:"..wks.name) .. '}"')
    premake.push("ProjectSection(SolutionItems) = preProject")
    for _, path in ipairs(wks.workspace_files) do
      premake.w(path.." = "..path)
    end
    premake.pop("EndProjectSection")
    premake.pop("EndProject")
  end
  base(wks)
end)

newoption {
	trigger = "glfwapi",
	description = "Which graphics API will be used with GLFW",
	default = "opengl",
	allowed = {
      { "opengl", "OpenGL" },
      { "vulkan", "Vulkan (Windows only)" },
   },
}

workspace "Engine"
	architecture "x64"
	startproject "Sandbox"
	cppdialect "C++17"
	systemversion "latest"

	configurations{
		"Debug",
		"Release"
	}
  
	defines{
		"FMT_HEADER_ONLY=1",
		"CORE_PLATFORM_WIN=1",
		"CORE_PLATFORM_LINUX=2",
		"GRAPHICS_API_OPENGL=1",
		"GRAPHICS_API_VULKAN=2",
		"GRAPHICS_API_D3D12=3",
		"GLM_ENABLE_EXPERIMENTAL",
	}
	
	workspace_files{
		"vendor/GLM/util/glm.natvis",
		"premake5.lua",
		"premakeDefines.lua",
	}

	filter "options:glfwapi=vulkan"
  		defines{
			"GLENGINE_GLFW_RENDERER=GRAPHICS_API_VULKAN",
			"VULKAN_BIN=\"C:/VulkanSDK/Bin\"",
			"VULKAN_GLSLC=VULKAN_BIN \"/glslc.exe\"",
  		}
	filter "options:glfwapi=opengl"
  		defines{
			"GLENGINE_GLFW_RENDERER=GRAPHICS_API_OPENGL",
		}

	filter "system:windows"
		disablewarnings {"4251"}
		defines {
			"CORE_PLATFORM=CORE_PLATFORM_WIN",
			"WIN32", 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS",
		}
		buildoptions
		{
			"/MP"
		}

	filter "system:linux"
		defines {
			"CORE_PLATFORM=CORE_PLATFORM_LINUX"
		}
		links { "stdc++fs" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"
		defines { 
			"DEBUG",
			"GL_ENGINE_DEBUG",
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		defines({ "NDEBUG" })

include "premakeDefines.lua"

IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"
IncludeDir["GLM"] = "vendor/GLM"
IncludeDir["GLI"] = "vendor/gli"
IncludeDir["Glad"] = "vendor/Glad/include"
IncludeDir["pugixml"] = "vendor/pugixml/src"
IncludeDir["fmt"] = "vendor/fmt/include"
IncludeDir["ImGui"] = "vendor/ImGui"
IncludeDir["ImGuiFileDialog"] = "vendor/ImGuiFileDialog"
IncludeDir["DevIL"] = "vendor/DevIL/DevIL/include"
IncludeDir["dirent"] = "vendor/dirent/include"
group "Assimp"
  include "vendor/projects/zlib"
  include "vendor/projects/irrXML"
  include "vendor/projects/Assimp"
group ""

VulkanSDKBase = "C:/VulkanSDK/"

group "Dependencies"
  include "vendor/GLFW"
  include "vendor/Glad"
  include "vendor/projects/pugixml"
  include "vendor/projects/ImGui"
  include "vendor/projects/ImGuiFileDialog"
  include "vendor/projects/DevIL"
  include "vendor/projects/libjpeg"
if _TARGET_OS ~= "linux" then
  include "vendor/projects/dirent"
end
group ""
group "Renderes"
	include "GLFWWindowManager"
if _TARGET_OS ~= "linux" then
	include "DX12Renderer"
end
group ""
group "Tools"
if (_OPTIONS["glfwapi"] ~= "opengl") then
	include "Tools/ShaderPreprocessor"
end
group ""

include "Core"
include "Sandbox"
include "Renderer"
include "GLRenderer"
include "GUI"
if (_OPTIONS["glfwapi"] ~= "opengl") then
	include "VulkanRenderer"
end
include "Entity"
include "Utils"
include "Physics"
