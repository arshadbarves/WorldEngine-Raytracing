-- premake5.lua
workspace "WorldEngine"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "WorldEngine"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "Build-WorldEngine-External.lua"
include "WorldEngine/Build-WorldEngine-App.lua"