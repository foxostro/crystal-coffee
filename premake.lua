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
	package.links = {
		"opengl32",
		"glu32",
		"glew32",
		"glut32",
		"png"
	}
    
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

