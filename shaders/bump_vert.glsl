varying vec3 LightDir;
varying vec3 EyeDir;

vec3 getTangent()
{
	vec3 tangent;
	vec3 c1 = cross(gl_Normal, vec3(0.0, 0.0, 1.0));
	vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0));

	if(length(c1) > length(c2)) {
		tangent = c1;
	} else {
		tangent = c2;
	}

	tangent = normalize(tangent);
	
	return tangent;
}

void main() 
{	
	vec3 v;
	
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	// Calculate coordinate-system for surface-local-space 
	vec3 n = vec3(gl_NormalMatrix * gl_Normal);
	vec3 t = vec3(gl_NormalMatrix * getTangent());
	vec3 b = cross(n, t);
	
	// Transform the light position from eye to object to surface-local
	vec3 LightPosition = vec3(gl_ModelViewMatrixInverse * gl_LightSource[0].position);
	v.x = dot(LightPosition, t);
	v.y = dot(LightPosition, b);
	v.z = dot(LightPosition, n);
	LightDir = normalize(v);
	
	// Transform the viewing direction to surface-local
	EyeDir = vec3(gl_ModelViewMatrix * gl_Vertex);
	v.x = dot(EyeDir, t);
	v.y = dot(EyeDir, b);
	v.z = dot(EyeDir, n);
	EyeDir = normalize(v);
}

