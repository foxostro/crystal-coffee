varying vec3 nLightDir;
varying vec3 nHalfVector;

void main() 
{	
	gl_Position = ftransform();
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
	
	// Transform the light direction to tangent-space
	nLightDir = mat3(tangent, binormal, gl_Normal) *
	            normalize(vec4(gl_ModelViewMatrixInverse * gl_LightSource[0].position - gl_Vertex).xyz);
	
	// Transform the halfVector to tangent-space
	nHalfVector = mat3(tangent, binormal, gl_Normal) * vec3(gl_ModelViewMatrixInverse * gl_LightSource[0].halfVector).xyz;
}

