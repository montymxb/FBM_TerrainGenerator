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

// eye space coordinates of the beginning of this volume
varying vec3 vESVolumeStart;
// dimensions of this volume
uniform vec3 uVolumeDimens;

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


// from S.O., what does dot() do? (dot product, yes), and fract() (fraction, yes)
// Random Poster: https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl#4275343
// Detailed Sourcing: https://stackoverflow.com/questions/12964279/whats-the-origin-of-this-glsl-rand-one-liner
// Supposedly from a 1998 Mathematical Statistics paper that has since been lost?
// Not Random, Hash Function (works for same X & Y)
float hash3(vec3 co) {
		// multiplies input by the seed
		// then converts number from 2D to 1D (via dot product)
		// arbitrary numbers 12.9898 and 78.233 chosen to avoid repitition
		// ~~~ used to multiply co.st by seed (a float)
		float t = dot(co.xyz, vec3(12.9898,78.233,34.424242));
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


// 1D noise function
// Based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
// referenced from: https://thebookofshaders.com/13/
float noise3D(vec3 v) {
    vec3 i = floor(v);
    vec3 f = fract(v);

    // calculate 8 corners of a 3D tile
    float a = hash3(i);
    float b = hash3(i + vec3(1.0, 0.0, 0.0));
    float c = hash3(i + vec3(0.0, 1.0, 0.0));
    float d = hash3(i + vec3(1.0, 1.0, 0.0));

		float a2 = hash3(i + vec3(0.0, 0.0, 1.0));
    float b2 = hash3(i + vec3(1.0, 0.0, 1.0));
    float c2 = hash3(i + vec3(0.0, 1.0, 1.0));
    float d2 = hash3(i + vec3(1.0, 1.0, 1.0));

		// calculate f^2 * (3.0 - 2.0f)
    vec3 u = f * f * (3.0 - 2.0 * f);

		// mix between a and b via u.x
		float h1 = mix(a, b, u.x) +
						// diff of c & a * u.y, multiplied by inverse of u.x
            (c - a)* u.y * (1.0 - u.x) +
						// add in diff of d - b * (u.x*u.y)
            (d - b) * u.x * u.y;

		// mix between a and b via u.x
		float h2 = mix(a2, b2, u.x) +
						// diff of c & a * u.y, multiplied by inverse of u.x
            (c2 - a2)* u.y * (1.0 - u.x) +
						// add in diff of d - b * (u.x*u.y)
            (d2 - b2) * u.x * u.y;

		// mix with regards to z
		float h = mix(h1, h2, u.z);

    return h;
}

// Fractal Brownian Motion
float fbm(vec3 v) {
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
		value += amplitude * noise3D(v);
		// scale frequency by lacunarity
		v *= lacunarity;
		// scale amplitude by gain
		amplitude *= gain;

	}

	return value;

}


// pow implementation
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


// is within the volume
bool isWithinVolume(vec3 p) {
	// determine that this point lies above and below the x,y,z positions that define this shape
	return (
		// within x coordinate
		p.x >= vESVolumeStart.x && p.x <= (vESVolumeStart.x + uVolumeDimens.x) &&
		// within y coordinate
		p.y >= vESVolumeStart.y && p.y <= (vESVolumeStart.y + uVolumeDimens.y) &&
		// within z coordinate
		p.z >= vESVolumeStart.z && p.z <= (vESVolumeStart.z + uVolumeDimens.z)
	);

}


void main() {

  vec4 color = vec4(0.0);

  vec3 rayCastPos = vMCPosition;
	// don't step it in too much off the bat
  //rayCastPos += vAdvanceVector;

  float f;

	/** TODO TEMP FOR BELOW
	f = fbm(rayCastPos.xyz + fbm(rayCastPos.xyz + fbm(rayCastPos.xyz + uSlowTime + uSeed))) * 2.0;
	if(f > 0.8) {
		// add color only if exceeds the threshold to display
		vec4 tColor = mix(
			vec4(0.0),
			vec4(1.0),
			(f - 0.8)
		);
		tColor *= 3.0;
		color += tColor;

	}
	/**/

	// step small increments until we exit the surface, then determine that is our point
	vec3 rayDir = normalize(vMCPosition) * 0.05;
	vec3 rayCastPoint = vMCPosition;
	//int stepCount = 0;
	while(isWithinVolume(rayCastPoint)) {
		rayCastPoint += rayDir;
		//if(stepCount > 50) {
			// safety break
			//break;
		//}
		//stepCount++;
	}

	// calc distance vector from vMCPosition to rayCastPoint that exited volume
	vec3 distanceV = vec3(vMCPosition.x - rayCastPoint.x, vMCPosition.y - rayCastPoint.y, vMCPosition.z - rayCastPoint.z);
	/*
	float dist = sqrt(
		pow(rayCastPoint.x - vMCPosition.x, 2) +
		pow(rayCastPoint.y - vMCPosition.y, 2) +
		pow(rayCastPoint.z - vMCPosition.z, 2)
	);
	*/

	// divide distance vector by desired step count
	// to calc step size
	int desiredSteps = 10;
	vec3 stepV = distanceV / float(desiredSteps);
	// reset raycast point
	rayCastPoint = vMCPosition;

	for(int x = 0; x < desiredSteps; x++) {
		if(color.a >= 1.0) {
			break;
		}

		// get fractal brownian motion val
		f = fbm(rayCastPos.xyz + fbm(rayCastPos.xyz + fbm(rayCastPos.xyz + uSlowTime + uSeed))) * 2.0;
		// only apply if greater than 0.8
		if(f > 0.8) {
			// add color only if exceeds the threshold to display
			vec4 tColor = mix(
				vec4(0.0),
				vec4(1.0),
				(f - 0.8)
			);
			tColor *= 0.333;
			color += tColor;

		}

		// advance rayCastPoint
		rayCastPoint += stepV;

	}

	if(color.a == 0.0) {
		// nothing to display...discard
		discard;
	}

  // hold alpha
  float alpha = color.a;

  // calc fragment lighting with color
  color = perFragmentLighting(color);

  gl_FragColor = vec4(color.rgb, alpha);

}
