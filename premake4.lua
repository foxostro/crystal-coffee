solution "crystal-coffee"
    configurations { "Debug", "Release" }

    -- A project defines one build target
    project "crystal-coffee"
        kind "ConsoleApp"
        language "C++"
        files { "**.h", "**.cpp" } -- recurse into subdirectories
        
        configuration "windows"  
            links { "opengl32",
            		"glu32",
            		"glut32",
            		"glew32",
            		"libpng" }
                    
        configuration "linux"
        	links { "GL",
                    "GLU",
                    "png",
                    "glut" }
                    
        configuration "vs2008"
            includedirs { "$(ProjectDir)", "$(ProjectDir)include" }
            libdirs { "$(ProjectDir)lib" }
            linkoptions { "/NODEFAULTLIB:MSVCRT", "/NODEFAULTLIB:LIBCMT" }
                    
        configuration "gmake"
            includedirs { "$(pwd)", path.join("$(pwd)", "include") }
            libdirs { path.join("$(pwd)", "lib") }
        
        configuration "Debug"
            defines { "DEBUG" }
            flags { "Symbols" }
            targetname "crystal-coffee-DEBUG"
        
        configuration "Release"
            defines { "NDEBUG" }
            flags { "Optimize" }
            targetname "crystal-coffee"
