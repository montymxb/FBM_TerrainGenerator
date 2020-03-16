/**
 * sky.frag
 *  Fragment shader for the sky
 */

// seed provided outside, better random
uniform float uSeed;
// extremely slow time
uniform float uSlowTime;

uniform float	uTime;

//uniform int activeWarpColor; // texture warp active
varying vec2  	vST;		// texture coords
varying vec3    vMCPosition;

// shaded normal, light, eye vectors
varying vec3 Ns;
varying vec3 Ls;
varying vec3 Es;

uniform float uAmbient, uDiffuse, uSpecular;
uniform vec3 SpecularColor;
uniform float Shininess;

uniform int uOctaves;

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

float pow(float base, int pow) {
  float val = base;
  if(pow == 0) {
    return 1.0;
  }
  for(int x = 1; x < pow; x++) {
    val*=base;
  }
  return val;
}

// per fragment lighting in the FRAG shader
vec4 perFragmentLighting(vec4 color) {
  vec3 Normal,Light,Eye;
  Normal = normalize(Ns);
  Light = normalize(Ls);
  Eye = normalize(Es);

  vec4 ambient = uAmbient * color;

  float d = max(dot(Normal,Light), 0.0);
  vec4 diffuse = uDiffuse * d * color;

  float s = 0.0;
  // only do specular if the light can see the point
  if(dot(Normal,Light) > 0.0) {
    vec3 ref = normalize(2.0 * Normal * dot(Normal,Light) - Light);
    s = pow(max(dot(Eye,ref), 0.0), Shininess);

  }

  vec4 specular = uSpecular * s * vec4(SpecularColor,1.0);

  return vec4(ambient.rgb + diffuse.rgb + specular.rgb, 1.0);
}


void main() {

  float f = fbm(vST + fbm(vST + fbm(vST + uSlowTime + uSeed))) * 2.0;

  if(f <= 0.8) {
    discard;
  }

  vec4 color = mix(
    vec4(0.0),
    vec4(1.0),
    (f - 0.8)
  );

  color *= 3.0;

  // hold alpha
  float alpha = color.a;

  // calc fragment lighting with color
  color = perFragmentLighting(color);

  gl_FragColor = vec4(color.rgb, alpha);

}
