-- WORKSPACE --
workspace 'Tuto'
configurations {'Debug', 'Release'}
location "build/" -- Generate all the files in build/

-- PROJECT --
project 'Tuto'
kind 'WindowedApp'
language 'C++'
cppdialect 'C++14'
flags {'Verbose', 'ShowCommandLine'}

files {
    'src/**.h',
    'src/**.hpp',
    'src/**.c',
    'src/**.cpp'
}

links {
    'glfw'
}

buildoptions {
    '-Wall',
    '-Winvalid-pch'
}

includedirs {
    'src'
}

pchheader 'pch.hpp'
pchsource 'pch.cpp'

objdir('build/obj')
targetdir('build/bin/%{cfg.buildcfg}')
targetname 'program'

filter {'configurations:Debug'}
defines {'DEBUG'}
symbols 'On'
optimize 'Off'

filter {'configurations:Release'}
defines {'NDEBUG'}
optimize 'On'

filter {'files:**.c'}
flags {'NoPCH'}
