solution "crystal-coffee"
    configurations { "Debug", "Release" }

    project "crystal-coffee"
        kind "ConsoleApp"
        language "C++"
        files { "**.h", "**.cpp" } -- recurse into subdirectories
		targetname "crystal-coffee"
		targetdir "./bin/"

        configuration "windows"  
            links { "opengl32",
            		"glu32",
            		"glut32",
            		"glew32",
            		"DevIL",
            		"SDL",
            		"SDLmain",
            		"ILU",
            		"ILUT" }
                    
        configuration "linux"
        	links { "GL",
                    "GLU",
            		"GLEW",
            		"SDL",
            		"SDLmain",
            		"IL",
            		"ILU",
            		"ILUT" }
                    
        configuration "gmake"
            includedirs { "." }

        configuration "vs2008"
            includedirs { "$(ProjectDir)", "$(ProjectDir)include-win32" }
            libdirs { "$(ProjectDir)lib" }
            linkoptions { "/NODEFAULTLIB:LIBCMT" }
            
        configuration { "vs2008", "Debug" } 
            linkoptions { "/NODEFAULTLIB:MSVCRT" }
        
        configuration "Debug"
            defines { "DEBUG" }
            flags { "Symbols" }
            targetprefix "DEBUG-"
        
        configuration "Release"
            defines { "NDEBUG" }
            flags { "OptimizeSpeed" }
