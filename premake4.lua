if not dofile 'premake_config.lua' then
    return
end

if _ACTION == "clean" then
    os.rmdir("build")
    return
elseif not _ACTION then
    return
end

solution "XWord"
    configurations(get_configurations())

    -- Output for premake4 (vs projects / makefiles, etc)
    location ("build/" .. _ACTION)

    -- Output for compiled files
    configuration "Debug"
        targetdir "build/bin/Debug"

    configuration "Release"
        targetdir "build/bin/Release"

    -- --------------------------------------------------------------------
    -- Debug / Release
    -- --------------------------------------------------------------------
    configuration "Release"
        defines { "NDEBUG" }
        flags { "Optimize" }
        libdirs { "build/bin/Release", "build/lib/Release" }
    configuration { "macoxs", "Release" }
        linkoptions "-S" -- Strip debug symbols

    configuration "Debug"
        defines { "DEBUG", "_DEBUG" }
        flags { "Symbols" }
        libdirs { "build/bin/Debug", "build/lib/Debug" }

    -- --------------------------------------------------------------------
    -- Platform-specific
    -- --------------------------------------------------------------------
    configuration "windows"
        defines { "WIN32", "_WINDOWS" }

    configuration "macoxs"
        platforms { "native", "x32", "x64" }

    configuration "linux"
        platforms { "native", "x32", "x64" }

    -- ------------------------------------------------------------------------
    -- General
    -- ------------------------------------------------------------------------

    configuration "vs*"
        defines { "VISUAL_STUDIO" }

    -- ------------------------------------------------------------------------
    -- General
    -- ------------------------------------------------------------------------

    configuration {}

    include "src" -- the XWord premake file
    include "puz" -- the puzzle library
    include "yajl"
    if not _OPTIONS["disable-lua"] then
        include "lua" -- lua libraries
    end


-- Mac stuff
if os.is("macosx") then
    -- Find a value in project.blocks
    local function get_key(p, k)
        if p[k] then return p[k] end
        for _, block in ipairs(p.blocks) do
            if block[k] then
                return block[k]
            end
        end
    end
    
    for _, p in pairs(solution().projects) do
        -- Put shared libs in XWord.app/Contents/Frameworks
        if get_key(p, "kind") == "SharedLib" then
            project(p.name)
                -- Set the output to the app bundle
                configuration { "macosx", "Debug" }
                    targetdir "bin/Debug/XWord.app/Contents/Frameworks"
                configuration { "macosx", "Release" }
                    targetdir "bin/Release/XWord.app/Contents/Frameworks"
                configuration { "macosx" }
                -- Set the install name
                linkoptions{ "-install_name @executable_path/../Frameworks/lib"..(get_key(p, "targetname") or p.name)..".dylib" }
        end
    end
end
