--
-- Premake 5.x build configuration script
--

build_base = "./"
project_name = "DrawDeviceForSteam"

	workspace (project_name)
		configurations { "Release", "Debug" }
		location   (build_base .. _ACTION)
		objdir     (build_base .. _ACTION .. "/obj")
		targetdir  ("bin")
		flags
		{
			"No64BitChecks",
			"StaticRuntime",	-- set /MT(d) option.

			"NoManifest",		-- Prevent the generation of a manifest.
			"NoMinimalRebuild",	-- Disable minimal rebuild.
--			"NoIncrementalLink",	-- Disable incremental linking.
			"NoPCH",		-- Disable precompiled header support.
		}
--		warnings "Extra"

		includedirs
		{
			"tp_stubz/",
			"simplebinder/",
			"drawdevicebase/",
		}

		filter "configurations:Debug"
			defines     "_DEBUG"
			optimize    "Debug"
			symbols     "On"
			targetsuffix "-d"

		filter "configurations:Release"
			defines     "NDEBUG"
			optimize    "On"

-- xp toolset support
		filter "action:vs2012"
			toolset "v110_xp"

		filter "action:vs2013"
			toolset "v120_xp"

		filter "action:vs2015"
			toolset "v140_xp"

		filter {}

	project (project_name)
		targetname  (project_name)
		language    "C++"
		kind        "SharedLib"

		files
		{
			"src/*.cpp",
			"src/*.hpp",
			"tp_stubz/tp_stub.cpp",
			"simplebinder/v2link.cpp",
			"drawdevicebase/*.cpp",
			"drawdevicebase/*.hpp",
		}
		links {
			"winmm",
			"d3d9",
--			"dxguid",
		}
