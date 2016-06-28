-- Common setup for lua shared libs

configuration "Debug"
    targetdir "../build/bin/Debug/scripts/libs"
configuration "Release"
    targetdir "../build/bin/Release/scripts/libs"
configuration {}


-- lua
defines { "LUA_LIB" }
-- libdirs { "../deps/luajit/lib" }
-- links { "lua51" }

configuration "windows"
    defines {
        "_USRDLL", "LUA_BUILD_AS_DLL",
    }
    links { "winmm" }

configuration "linux"
    includedirs { "/usr/include/lua5.1" }
    defines { "LUA_USE_LINUX" }
    links { "dl", "lua5.1" }

configuration "Debug"
    defines { "LUA_USE_APICHECK" }

configuration {}
