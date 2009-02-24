varying vec3 nLightDir;
varying vec3 ecNormal;

void main() 
{
	vec3 oLightPos, oLightDir;
	
	gl_Position = ftransform();
	ecNormal = gl_NormalMatrix * gl_Normal; // normal goes to eye-space
	gl_TexCoord[0] = gl_MultiTexCoord0;
	
	// Calculate the tangent and binormal
	vec3 tangent; 
	vec3 binormal; 
	vec3 c1 = cross(gl_Normal, vec3(0.0, 0.0, 1.0));
	vec3 c2 = cross(gl_Normal, vec3(0.0, 1.0, 0.0));

	if(length(c1) > length(c2)) {
		tangent = c1;
	} else {
		tangent = c2;
	}

	tangent = normalize(tangent);

	binormal = cross(tangent, gl_Normal); 
	binormal = normalize(binormal);
	
	// Get the light position and direction in object-space
	oLightPos = vec4(gl_ModelViewMatrixInverse * gl_LightSource[0].position).xyz;
	oLightDir = oLightPos - gl_Vertex.xyz;
	oLightDir = normalize(oLightDir);
	
	// Get the light position and direction in tangent-space
	nLightDir = mat3(tangent, binormal, gl_Normal) * oLightDir;
	nLightDir = normalize(nLightDir);
}

