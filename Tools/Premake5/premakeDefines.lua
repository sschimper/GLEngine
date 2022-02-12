outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

function PrecompiledHeaders(projectName)
	pchheader (tostring(projectName).."Stdafx.h")
	pchsource (tostring(projectName).."Stdafx.cpp")

	files 
	{
		tostring(projectName).."Stdafx.cpp",
		tostring(projectName).."Stdafx.h"
	}

end

function ProjectFiles(projectName)
	files
	{
		tostring(projectName).."/**.h",
		tostring(projectName).."/**.cpp",
		tostring(projectName).."/**.inl",
		"premake5.lua",
	}

	includedirs
	{
		".",
	}
end

function Link(projectName)
	includedirs("%{wks.location}/" .. projectName)
	-- error("%{prj.location}")
	libdirs { "%{wks.location}/bin/" .. outputdir .. "/" .. projectName }


	links
	{
		projectName,
	}

if _TARGET_OS == "windows" then
	postbuildcommands
	{
		("{COPY} \"%{wks.location}/bin/" .. outputdir .. "/" .. projectName .. "/".. projectName ..".dll\" \"%{cfg.buildtarget.directory}\"")
	}
end
end

function LinkDependency(depName)
	includedirs("%{wks.location}/%{IncludeDir."..depName.."}")
	libdirs {"%{wks.location}/bin/"..outputdir.."/vendor/"..depName.."/"}
	links
	{
		depName,
	}
	if _TARGET_OS == "windows" then
	  if NonDllLib[depName] == nil then
		postbuildcommands
		{
			("{COPY} \"%{wks.location}/bin/" .. outputdir .. "/vendor/" .. depName .. "/".. depName ..".dll\" \"%{cfg.buildtarget.directory}\"")
		}
	  else
		postbuildcommands
		{
			("{COPY} \"%{wks.location}/bin/" .. outputdir .. "/vendor/" .. depName .. "/".. depName ..".lib\" \"%{cfg.buildtarget.directory}\"")
		}
	  end
	end
end

function SetupProject(projectName)
	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/obj/" .. outputdir .. "/%{prj.name}")
	ProjectFiles(projectName)
	-- everybody needs guid
	includedirs{"%{wks.location}/%{IncludeDir.crossguid}"}
	defines
	{
		"BUILD_"..string.upper(tostring(projectName)).."_DLL",
	}
end