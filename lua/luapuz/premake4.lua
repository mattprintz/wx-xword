project "luapuz"
    kind "SharedLib"
    language "C++"
    files {
        "*.cpp",
        "*.hpp",
        "bind/*.cpp",
        "bind/*.hpp",
    }

    -- Common lua stuff
    dofile "../premake4_lualib.lua"

    configuration "Debug"   targetdir "../../build/bin/Debug"
    configuration "Release" targetdir "../../build/bin/Release"
    configuration {}

    includedirs { "../.." }
    links { "puz" }

    configuration "windows"
        defines {
            "PUZ_API=__declspec(dllimport)",
            "LUAPUZ_API=__declspec(dllexport)",
        }

    -- Disable some warnings
    configuration "vs*"
        buildoptions {
            "/wd4800", -- implicit conversion to bool
            "/wd4244", -- conversion: possible loss of data
            "/wd4251", -- class needs dll-interface
        }


    configuration "linux"
        includedirs { "/usr/include/lua5.1" }
        defines {
            "LUA_USE_LINUX",
            "LUA_COMPAT_MODULE",
            "PUZ_API=\"\"",
            "LUAPUZ_API=\"\""
        }
        links { "dl", "lua5.1" }

    configuration "macosx"
        defines {
            [[PUZ_API=\"\"]],
            [[LUAPUZ_API=\"\"]]
        }
