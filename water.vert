//#version 210

// seed provided outside, better random
uniform float uSeed;
// extremely slow time
uniform float uSlowTime;

uniform float	uTime;		// "Time", from Animate( )

varying vec2  	vST;		// texture coords
varying vec3    vMCPosition; // model coords

// shaded normal, light, eye vectors
varying vec3 Ns;
varying vec3 Ls;
varying vec3 Es;

// light position
uniform float LightX;
uniform float LightY;
uniform float LightZ;

uniform int uOctaves;

uniform float uWaterHeight;

// approximate eye light position
vec3 eyeLightPosition = vec3(LightX,LightY,LightZ);

const float PI = 	3.14159265;

// from S.O., what does dot() do? (dot product, yes), and fract() (fraction, yes)
// Random Poster: https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl#4275343
// Detailed Sourcing: https://stackoverflow.com/questions/12964279/whats-the-origin-of-this-glsl-rand-one-liner
// Supposedly from a 1998 Mathematical Statistics paper that has since been lost?
// Not Random, Hash Function (works for same X & Y)
float hash(vec2 co) {
		// multiplies input by the seed
		// then converts number from 2D to 1D (via dot product)
		// arbitrary numbers 12.9898 and 78.233 chosen to avoid repitition
		// ~~~ used to multiply co.st by seed (a float)
		float t = dot(co.st, vec2(12.9898,78.233));
		// then takes the sin of that number
		// then multiplies by 43758.5453, which amplifies the error of the sin function (based on local implementation)
		float u = sin(t) * 43758.5453123;
		// then returns the fractional component of that number, focusing further on the error
		// overall, this is a dubious hash function because sin() is platform specific, and may not be consistent
    return fract(u);

}


// 1D noise function
// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
// referenced from: https://thebookofshaders.com/13/
float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // calculate 4 corners of a 2D tile
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));

		// calculate f^2 * (3.0 - 2.0f)
    vec2 u = f * f * (3.0 - 2.0 * f);

		// mix between a and b via u.x
		float h = mix(a, b, u.x) +
						// diff of c & a * u.y, multiplied by inverse of u.x
            (c - a)* u.y * (1.0 - u.x) +
						// add in diff of d - b * (u.x*u.y)
            (d - b) * u.x * u.y;

    return h;
}

// Fractal Brownian Motion
float fbm(vec2 v) {
	// number of iterations
	int octaves 		= uOctaves;
	// initial value
	float value 		= 0.0;
	// initial amp at half
	float amplitude = 0.5;

	// regular step to increase freqency by
	float lacunarity = 2.0;
	// amplitude modification
	float gain = 0.5;

	for(int x = 0; x < octaves; x++) {
		// add noise of 'v' scaled by amplitude
		value += amplitude * noise(v);

		// scale frequency by lacunarity
		v *= lacunarity;
		// scale amplitude by gain
		amplitude *= gain;
	}

	return value;

}


void perFragmentLighting(vec4 ECPosition, vec3 adjustedNormal) {
	vec3 normal = normalize(gl_NormalMatrix * adjustedNormal);

	// set surface normal
	Ns = normal;

	// calc vector from point to light
	Ls = eyeLightPosition - ECPosition.xyz;

	// vec from point to eye position
	Es = vec3(0.0, 0.0, 0.0) - ECPosition.xyz;

	// adjust existing normals
	/*
	Ns = normal * Ns;
	Ls = normal * Ls;
	Es = normal * Es;
	/**/

}


// returns the water 'Y' for a given point on a 2D grid
float getWaterY(vec2 p) {
	float fval = fbm(p * 20.0 + uSlowTime * 5.0 + uSeed) * 0.025;
	float sval = sin(uTime * 2.0 * 3.14159 + ((p.x + 0.31) * (p.y+0.89) * 11.429)) * 0.004;
	return fval+sval;
}


// calculates the surface normal that should be used
vec3 calcNormal(vec3 p1) {
	// calculate 2 other points slightly farther along s and t
  //p1.y = 1.0;
	vec3 p2 = p1;
	vec3 p3 = p1;

	// slightly shift p2's x up and calc new y
	p2.x += 0.025;
	p2.y = getWaterY(p2.xz);
	//p2.y = (fbm(p2.xz * 20.0 + modifier + uSeed) * 0.05) + (sin(uTime * 2.0 * 3.14159 + (p2.x * 20.0)) * 0.01);
  //p2.y += fbm(p2.xz + fbm(p2.xz + fbm(p2.xz + modifier + uSeed))) * 0.05;
	// slightly shift p3's z up and calc new y
	p3.z += 0.025;
	p3.y = getWaterY(p3.xz);
	//p3.y = (fbm(p3.xz * 20.0 + modifier + uSeed) * 0.05) + (sin(uTime * 2.0 * 3.14159 + (p2.x * 20.0)) * 0.01);
  //p3.y += fbm(p3.xz + fbm(p3.xz + fbm(p3.xz + modifier + uSeed))) * 0.05;

	// calculate cross of vector(p1,p2) and vector(p1,p3)
	vec3 v1 = p1 - p2;
	vec3 v2 = p1 - p3;
	vec3 normal = normalize(cross(v2,v1));

	// return as new normal
	return normal;
}


void main() {

	vec4 ECPosition = gl_ModelViewMatrix * gl_Vertex;

	vST = gl_MultiTexCoord0.st;
	//vec3 vert = ECPosition.xyz;
	vec3 vert = gl_Vertex.xyz;

	// pull aside S and T for noise
  // f(p) = fbm( p + fbm( p + fbm( p ) ) )
  //float ff = fbm(vert.xz + fbm(vert.xz + fbm(vert.xz + uSlowTime + uSeed))) * 0.05;
	float oldY = vert.y;
  vert.y = getWaterY(vert.xz);

	vec3 adjustedNormal = calcNormal(vert);

	// adjustment to mesh with estimated land height
	vert.y = oldY;
  vert.y += (0.43 + uWaterHeight);

	// setup perfragment lighting in vertex shader
	perFragmentLighting(ECPosition, adjustedNormal);

	// store model coordinates for use in frag shader
	vMCPosition = vert.xyz;

	gl_Position = gl_ModelViewProjectionMatrix * vec4(vert,1.0);

}
