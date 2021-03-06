-- Generate tar.gz files for all packages
-- Generate packages_windows.lua file
-- Generate readme.md for sourceforge.
-- NOTE: don't forget to update xword_version

xword_version = "0.6.1"

local lfs = require 'lfs'

-- Save current directory and change package.path and package.cpath
local currentdir = lfs.currentdir()
lfs.chdir('../scripts')
local scriptsdir = lfs.currentdir()
lfs.chdir('../../files/scripts')
local oldscriptsdir = lfs.currentdir()
lfs.chdir(currentdir)

package.path = table.concat(
    {
        scriptsdir.."\\?.lua",
        scriptsdir.."\\?\\init.lua",
        scriptsdir.."\\?\\?.lua",
        scriptsdir.."\\libs\\?.lua",
        scriptsdir.."\\libs\\?\\init.lua",
        scriptsdir.."\\libs\\?\\?.lua",
    },
    ';'
)

package.cpath = table.concat(
    {
        scriptsdir.."\\?.dll",
        scriptsdir.."\\?51.dll",
        scriptsdir.."\\libs\\?.dll",
        scriptsdir.."\\libs\\?51.dll",
    },
    ';'
)


local serialize = require 'serialize'
local join = require 'pl.path'.join
local startswith = require 'pl.stringx'.startswith
local endswith = require 'pl.stringx'.endswith

local function gen_packages(outdir)
    -- Walk scriptsdir
    local packages = {
        xword = {
            name = "XWord",
            version = xword_version,
            download = string.format("http://sourceforge.net/projects/wx-xword/files/Binary/XWord_%s.exe", xword_version)
        }
    }
    function add_info(filename)
        local info = serialize.loadfile(filename)
        if info then
            if info.name and info.requires and info.version and info.packagename then
                -- Make sure we don't have duplicates
                for _, pkg in ipairs(packages) do
                    if pkg.name == info.name and pkg.version == info.version then
                        return info
                    end
                end
                -- Add info to the package table
                info.download = string.format("http://sourceforge.net/projects/wx-xword/files/scripts/%s_%s.tar.gz", info.packagename, info.version)
                table.insert(packages, info)
            end
            return info
        end
    end
    local readme = {}
    lfs.mkdir(join(outdir, "_temp_packages"))
    -- Current packages
    for name in lfs.dir(scriptsdir) do
        local dirname = join(scriptsdir, name)
        if not startswith(name, '.') and name ~= 'xword' and name ~= 'libs'
            and lfs.attributes(dirname, 'mode') == 'directory'
            and not startswith(name, "_")
        then
            local info = add_info(join(dirname, 'info.lua'))
            if info then
                if info.name and info.requires and info.version and info.packagename then
                    -- Add info to readme.md
                    table.insert(readme, string.format("%s (%s)\n====\n%s\n\n----\n\n", info.name, info.version, info.description))
                    -- Copy the files to a temp directory so that we can
                    -- create the scripts/packagename directory structure.
                    os.execute(string.format(
                        'xcopy /e "%s\\*.*" "%s\\"',
                        join(scriptsdir, name), join(outdir, "_temp_packages", "scripts", name)
                    ))
                    os.execute(string.format(
                        'copy "%s" "%s\\"',
                        join(scriptsdir, name, 'info.lua'), join(outdir, "_temp_packages")
                    ))
                    -- Pack the file in a tar
                    local archive_name = join(outdir, name..'_'..info.version)
                    local rc = os.execute(string.format(
                        "%s a -r -x!.* %s.tar %s/*",
                        '7za.exe',
                        archive_name,
                        join(outdir, "_temp_packages")
                    ))
                    -- Remove the temp directory
                    os.execute(string.format(
                        'rmdir /s /q "%s"',
                        join(outdir, "_temp_packages")
                    ))
                    if rc ~= 0 then
                        print("Error compressing files for package "..name)
                    else
                        -- Zip the tar
                        local rc = os.execute(string.format(
                            "%s a %s.tar.gz %s.tar",
                            '7za.exe',
                            archive_name,
                            archive_name
                        ))
                        if rc ~= 0 then
                            print("Error compressing files for package "..name)
                            os.remove(archive_name..'.tar.gz')
                        else
                            print("Package "..name.." compressed to "..archive_name..".tar.gz")
                        end
                    end
                    -- Remove the tar
                    os.remove(archive_name..'.tar')
                else
                    print("Package "..name.." has an incomplete info.lua file")
                end
            else
                print("Package "..name.." is missing info.lua")
            end
        end
    end
    -- Old Packages
    for name in lfs.dir(oldscriptsdir) do
        local archive_name = name:match("(.-)%.tar%.gz") or ""
        if archive_name
            and lfs.attributes(join(oldscriptsdir, archive_name..'.tar.gz'), 'mode') == 'file'
        then
            -- Unzip the archive
            local rc = os.execute(string.format(
                "%s e %s.tar.gz -o\"%s\"",
                '7za.exe',
                join(oldscriptsdir, archive_name),
                join(outdir, "_temp_packages")
            ))
            if rc ~= 0 then
                print("Error extracting package "..archive_name)
            else
                -- Extract the info file
                local rc = os.execute(string.format(
                    "%s e %s.tar info.lua -o\"%s\"",
                    '7za.exe',
                    join(outdir, "_temp_packages", archive_name),
                    join(outdir, "_temp_packages")
                ))
                if rc ~= 0 then
                    print("No info.lua file: "..archive_name)
                else
                    local info_filename = join(outdir, "_temp_packages", "info.lua")
                    add_info(info_filename)
                    os.remove(info_filename)
                end
                os.remove(join(outdir, "_temp_packages", archive_name .. '.tar'))
            end
        end
    end
    serialize.pdump(packages, join(outdir, 'packages_windows.lua'))
    local f = io.open(join(outdir, 'readme.md'), 'w')
    f:write(table.concat(readme, '\n'))
    f:close()
    print("Packages written to "..join(outdir, 'packages_windows.lua'))
end

lfs.mkdir(join(lfs.currentdir(), 'packages'))
gen_packages(join(lfs.currentdir(), 'packages'))
print("Press any key to continue")
io.read()
