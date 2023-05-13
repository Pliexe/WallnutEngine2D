-- HeliosEngine - A cross-platform game engine
-- HeliosEditor - A cross-platform game editor using HeliosEngine
-- HeliosEditor dynamically links to HeliosEngine



workspace "HeliosEngine"
    architecture "x64"
    startproject "HeliosEditor"

    configurations
    {
        "Debug",
        "Release"
    }

    flags
	{
		"MultiProcessorCompile"
	}

    defines
    {
        "HELIOS_IMGUI_ENABLED",
    }

outdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["ImGui"] = "%{wks.location}/HeliosEngine/vendor/imgui"
IncludeDir["ImGuiMisc"] = "%{wks.location}/HeliosEngine/vendor/imgui/misc/cpp"
IncludeDir["ImGuiBackends"] = "%{wks.location}/HeliosEngine/vendor/imgui/backends"
IncludeDir["Yaml"] = "%{wks.location}/HeliosEngine/vendor/yaml-cpp/include"
IncludeDir["ProjectManager"] = "%{wks.location}/ProjectManager/src"
IncludeDir["GLFW"] = "%{wks.location}/HeliosEngine/vendor/glfw/include"
IncludeDir["GLAD"] = "%{wks.location}/HeliosEngine/vendor/glad/include"

group "Dependencies"
    include "HeliosEngine/vendor"

    project "ProjectManager"
        location "ProjectManager"
        kind "SharedLib"
        language "C++"
        staticruntime "off"
        cppdialect "C++17"

        targetdir ("%{wks.location}/bin/" .. outdir .. "/%{prj.name}")
        objdir ("%{wks.location}/bin-int/" .. outdir .. "/%{prj.name}")

        files
        {
            "ProjectManager/src/**.c",
            "ProjectManager/src/**.cpp",
        }

        includedirs
        {
            "ProjectManager/src",
            "%{wks.location}/HeliosEditor/src",
            "%{IncludeDir.Yaml}",
        }

        links
        {
            "Yaml",
        }

        defines
        {
            "YAML_CPP_STATIC_DEFINE",
        }

        flags { "NoPCH" }

        postbuildcommands
        {
            ("{COPY} %{cfg.buildtarget.relpath} %{wks.location}/bin/" .. outdir .. "/HeliosEditor")
        }

        filter "system:windows"
            systemversion "latest"
            defines { "HELIOS_PLATFORM_WINDOWS", "HELIOS_DEBUG" }

        filter "configurations:Debug"
            defines "HELIOS_DEBUG"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            defines "HELIOS_RELEASE"
            runtime "Release"
            optimize "on"
        
group ""

    include "HeliosEngine"
    include "HeliosEditor"

newaction {
    trigger = "clean",
    description = "Remove all binaries and intermediate files, and project files",
    execute = function ()
        print("Removing all binaries and intermediate files")
        os.rmdir('bin')
        os.rmdir('bin-int')
        print("Removing all project files")
        os.remove('**.sln')
        os.remove('**.vcxproj')
        os.remove('**.vcxproj.*')
    end
}