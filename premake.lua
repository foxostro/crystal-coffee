project.name = "crystal-coffee"

-- Main Game Application -----------------------------------------------------

package = newpackage()
package.name = "crystal-coffee"
package.kind = "exe"
package.language = "c++"

package.files = {
	matchrecursive("*.h", "*.cpp")
}

if OS == "windows" then    
	package.includepaths = { "$(ProjectDir)", "$(ProjectDir)include" }
	
	package.libpaths = { "$(ProjectDir)lib" }
	
	package.links = {
		"glut32",
		"glew32",
		"libpng"
	}
	
	package.linkoptions = { "/NODEFAULTLIB:LIBCMT /NODEFAULTLIB:MSVCRT" }
    
elseif OS == "linux" then
	package.links = {
		"GL",
		"GLU",
		"GLEW",
		"png",
		"glut"
	}
	
	package.buildoptions = { "-I\"$(shell pwd)\"" }
else
	error("Unsupported Operating System: " .. OS)
end

