-- premake5.lua
workspace "libgff"
   configurations { "Debug", "Release" }
   location ( "build/" )

project "gff"
   kind "SharedLib"
   language "C"
   targetdir "lib/"

   includedirs { "include/", "ext/" }

   files { "include/**.h", "src/**.c" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

project "xmi2mid"
   kind "SharedLib"
   language "C"
   targetdir "lib/"

   includedirs { "include/", "ext/" }

   files { "ext/*.h", "ext/*.c" }

   filter "configurations:Debug"
      defines { "DEBUG" }
      symbols "On"

   filter "configurations:Release"
      defines { "NDEBUG" }
      optimize "On"

local tests = {
    "ds1-basic",
    "ds1-gpldata",
    "ds1-image",
    "ds1-manager",
    "ds1-region",
    "ds1-resource",
    "ds1-segobjex",
}

for _, test in pairs(tests) do
    project(test)
        kind "ConsoleApp"
        language "C"
        targetdir "bin/"
        includedirs { "include/", "ext/", "unity/" }
        links {"gff", "xmi2mid"}

        files {"test/" .. test .. ".c", "unity/unity.c"}
end
