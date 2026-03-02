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

-- Include directories relative to root
IncludeDir = {}
IncludeDir["GLFW"] = "Engine/vendor/glfw/include"
IncludeDir["Glad"] = "Engine/vendor/glad/include"
IncludeDir["glm"] = "Engine/vendor/glm"
IncludeDir["ImGui"] = "Engine/vendor/imgui"
IncludeDir["ImGuizmo"] = "Engine/vendor/ImGuizmo"

-- GLFW static library project
project "GLFW"
	location "Engine/vendor/glfw"
	kind "StaticLib"
	language "C"
	staticruntime "On"

	targetdir ("bin/" ..outputdir.. "/%{prj.name}")
	objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

	files
	{
		"Engine/vendor/glfw/include/GLFW/glfw3.h",
		"Engine/vendor/glfw/include/GLFW/glfw3native.h",
		"Engine/vendor/glfw/src/glfw_config.h",
		"Engine/vendor/glfw/src/context.c",
		"Engine/vendor/glfw/src/init.c",
		"Engine/vendor/glfw/src/input.c",
		"Engine/vendor/glfw/src/monitor.c",
		"Engine/vendor/glfw/src/null_init.c",
		"Engine/vendor/glfw/src/null_joystick.c",
		"Engine/vendor/glfw/src/null_monitor.c",
		"Engine/vendor/glfw/src/null_window.c",
		"Engine/vendor/glfw/src/platform.c",
		"Engine/vendor/glfw/src/vulkan.c",
		"Engine/vendor/glfw/src/window.c"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/utf-8" }

		files
		{
			"Engine/vendor/glfw/src/win32_init.c",
			"Engine/vendor/glfw/src/win32_joystick.c",
			"Engine/vendor/glfw/src/win32_module.c",
			"Engine/vendor/glfw/src/win32_monitor.c",
			"Engine/vendor/glfw/src/win32_time.c",
			"Engine/vendor/glfw/src/win32_thread.c",
			"Engine/vendor/glfw/src/win32_window.c",
			"Engine/vendor/glfw/src/wgl_context.c",
			"Engine/vendor/glfw/src/egl_context.c",
			"Engine/vendor/glfw/src/osmesa_context.c"
		}

		defines
		{
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"

-- ImGui static library project
project "ImGui"
	location "Engine/vendor/imgui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir ("bin/" ..outputdir.. "/%{prj.name}")
	objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

	files
	{
		"Engine/vendor/imgui/imgui.h",
		"Engine/vendor/imgui/imgui.cpp",
		"Engine/vendor/imgui/imgui_demo.cpp",
		"Engine/vendor/imgui/imgui_draw.cpp",
		"Engine/vendor/imgui/imgui_tables.cpp",
		"Engine/vendor/imgui/imgui_widgets.cpp",
		"Engine/vendor/imgui/imstb_rectpack.h",
		"Engine/vendor/imgui/imstb_textedit.h",
		"Engine/vendor/imgui/imstb_truetype.h",
		"Engine/vendor/imgui/backends/imgui_impl_glfw.h",
		"Engine/vendor/imgui/backends/imgui_impl_glfw.cpp",
		"Engine/vendor/imgui/backends/imgui_impl_opengl3.h",
		"Engine/vendor/imgui/backends/imgui_impl_opengl3.cpp",
		"Engine/vendor/imgui/backends/imgui_impl_opengl3_loader.h"
	}

	includedirs
	{
		"Engine/vendor/imgui",
		"%{IncludeDir.GLFW}"
	}

	defines
	{
		"IMGUI_API=__declspec(dllexport)"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/utf-8" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"

-- ImGuizmo static library project
project "ImGuizmo"
	location "Engine/vendor/ImGuizmo"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir ("bin/" ..outputdir.. "/%{prj.name}")
	objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

	files
	{
		"Engine/vendor/ImGuizmo/ImGuizmo.h",
		"Engine/vendor/ImGuizmo/ImGuizmo.cpp"
	}

	includedirs
	{
		"Engine/vendor/ImGuizmo",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends"
	}

	defines
	{
		"IMGUI_API=__declspec(dllexport)"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/utf-8" }

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		runtime "Release"
		optimize "On"

-- Engine DLL project
project "Engine"
	location "Engine"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir ("bin/" ..outputdir.. "/%{prj.name}")
	objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"Engine/vendor/glad/src/gl.c"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends",
		"%{IncludeDir.ImGuizmo}"
	}

	links
	{
		"GLFW",
		"ImGui",
		"ImGuizmo",
		"opengl32.lib"
	}

	defines
	{
		"GLFW_INCLUDE_NONE",
		"IMGUI_API=__declspec(dllexport)",
		"GLM_ENABLE_EXPERIMENTAL"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/utf-8" }
		disablewarnings { "4251" }

		defines
		{
			"ENGINE_PLATFORM_WINDOWS",
			"ENGINE_BUILD_DLL"
		}

		postbuildcommands
		{
			("{MKDIR} ../bin/" ..outputdir.. "/Sandbox"),
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" ..outputdir.. "/Sandbox" )
		}

	filter "configurations:Debug"
		defines "ENGINE_DEBUG"
		symbols "On"
	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		optimize "On"
	filter "configurations:Dist"
		defines "ENGINE_DIST"
		optimize "On"

-- Sandbox application project
project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir ("bin/" ..outputdir.. "/%{prj.name}")
	objdir ("bin-int/" ..outputdir.. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Engine/vendor/spdlog/include",
		"Engine/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGui}/backends",
		"%{IncludeDir.ImGuizmo}"
	}

	links
	{
		"Engine"
	}

	filter "system:windows"
		systemversion "latest"
		buildoptions { "/utf-8" }

		defines
		{
			"ENGINE_PLATFORM_WINDOWS",
			"IMGUI_API=__declspec(dllimport)",
			"GLM_ENABLE_EXPERIMENTAL"
		}

	filter "configurations:Debug"
		defines "ENGINE_DEBUG"
		symbols "On"
	filter "configurations:Release"
		defines "ENGINE_RELEASE"
		optimize "On"
	filter "configurations:Dist"
		defines "ENGINE_DIST"
		optimize "On"