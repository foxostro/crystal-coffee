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
		"opengl32",
		"glu32",
		"glut32",
		"glew32",
		"libpng"
	}
    
elseif OS == "linux" then
	package.links = {
		"GL",
		"GLU",
		"GLEW",
		"png",
		"glut"
	}
	
	package.buildoptions = { "-I\"$(shell pwd)\"", "-I\"$(shell pwd)/include\"" }
else
	error("Unsupported Operating System: " .. OS)
end

