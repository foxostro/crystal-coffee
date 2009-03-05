solution "crystal-coffee"
    configurations { "Debug", "Release" }

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
                    
        configuration "gmake"
            includedirs { "$(pwd)", path.join("$(pwd)", "include") }
            libdirs { path.join("$(pwd)", "lib") }
                    
        configuration "vs2008"
            includedirs { "$(ProjectDir)", "$(ProjectDir)include" }
            libdirs { "$(ProjectDir)lib" }
            linkoptions { "/NODEFAULTLIB:LIBCMT" }
            
        configuration { "vs2008", "Debug" } 
            linkoptions { "/NODEFAULTLIB:MSVCRT" }
        
        configuration "Debug"
            defines { "DEBUG" }
            flags { "Symbols" }
            targetname "crystal-coffee-DEBUG"
        
        configuration "Release"
            defines { "NDEBUG" }
            flags { "OptimizeSpeed" }
            targetname "crystal-coffee"
