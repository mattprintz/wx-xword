project "wxlua"
    -- --------------------------------------------------------------------
    -- General
    -- --------------------------------------------------------------------
    kind "StaticLib"
    language "C++"
    files { "*", "debug/*", "debugger/*" }
    includedirs {
        "../wxbind/setup",
--        "../../deps/luajit/include",
        "..",
        "."
    }
    libdirs { "../../deps/luajit/lib" }

    configuration "Debug"   targetdir "../../build/lib/Debug"
    configuration "Release" targetdir "../../build/lib/Release"

    configuration "linux"
        includedirs { "/usr/include/lua5.1" }
--        defines { "LUA_USE_LINUX", "LUA_COMPAT_MODULE" }
        defines { "LUA_USE_LINUX" }
        links { "dl", "lua5.1" }

    -- --------------------------------------------------------------------
    -- Platform-specific
    -- --------------------------------------------------------------------
    configuration "windows"
        defines { "_LIB" }

    -- Disable some warnings
    configuration "vs*"
        buildoptions {
            "/wd4800", -- implicit conversion to bool
            "/wd4390", -- empty control statement
        }

    -- wxWidgets
    dofile "../../premake4_wxdefs.lua"
