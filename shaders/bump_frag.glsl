varying vec3 normal, vp;
varying float d;

uniform sampler2D diffuse_map;
uniform sampler2D bump_map;

vec4 getAmbientGlobal()
{
	/* light model (i.e. global) ambient term */
	return gl_LightModel.ambient * gl_FrontMaterial.ambient;
}

vec4 getAmbient()
{
	return gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
}

vec4 getDiffuse(float nDotVP)
{
	return gl_FrontMaterial.diffuse *
	       gl_LightSource[0].diffuse *
	       nDotVP;
}

float getAttenuation(float d)
{		
	/* standard light attenuation equation */
	return 1.0 / (gl_LightSource[0].constantAttenuation +
	              gl_LightSource[0].linearAttenuation * d +
	              gl_LightSource[0].quadraticAttenuation * d * d);
}

vec4 getSpecular(vec3 n)
{
	float nDotHV;
	vec3 halfVector;
		
	halfVector = normalize(gl_LightSource[0].halfVector.xyz);
	
	nDotHV = max(dot(n, halfVector),0.0);
	
	return gl_FrontMaterial.specular *
	       gl_LightSource[0].specular *
	       pow(nDotHV, gl_FrontMaterial.shininess);
}

void main()
{
	vec3 n;
	float nDotVP;
	vec4 diff_tex_color;
	
	/* Fetch the texture colors first so it can read from memory while we
	 * perform other calculations
	 */
	diff_tex_color = texture2D(diffuse_map, gl_TexCoord[0].st);
	
	/* renormalize the interpolated normal */
	n = normalize(normal);
	
	/* Calculate the Lambert term */
	nDotVP = max(dot(n, normalize(vp)), 0.0);

	if (nDotVP > 0.0) {		/* shade the fragment using per-pixel point lighting */
		gl_FragColor = getAmbientGlobal()*diff_tex_color +
		               getAttenuation(d) * (getAmbient()*diff_tex_color +
		                                    getDiffuse(nDotVP)*diff_tex_color +
		                                    getSpecular(n));
	} else {
		/* in darkness, the fragment is only lit by the global ambient term */
		gl_FragColor = getAmbientGlobal() * diff_tex_color;
	}
}
