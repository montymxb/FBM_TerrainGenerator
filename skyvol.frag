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
varying vec2 vST;		// texture coords
varying vec3 vMCPosition; // model coords
varying vec4 vECPosition; // eye coords (camera coordinates)
varying vec3 vGLPosition; // use gl_Position

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

// start & dimensions of this volume, adjusted to eye space
varying vec3 vVolumeStart;
varying vec3 vVolumeDimens;

// inverse of model view matrix
varying mat4 vModelViewMatrix_Inverse;

// eye position
uniform vec3 uEyeAt;

// eye coordinate lights
varying vec3 vECLight;


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


// replacement for the shaded normal from the vertex shader
// instead computes the normal with respect to the termination positions of 3 rays
vec3 ReplaceNormal;

// used to compute modification of lighting
float lightAlpha = 0.0;


// per fragment lighting in the FRAG shader
vec4 perFragmentLighting(vec4 color) {
  vec3 Normal,Light,Eye;
  Normal = normalize(ReplaceNormal);
  Light = normalize(Ls);
  Eye = normalize(Es);

  vec4 ambient = uAmbient * color * lightAlpha;

  float d = max(dot(Normal,Light), 0.0);
  vec4 diffuse = uDiffuse * d * color;

  float s = 0.0;
  // only do specular if the light can see the point
  if(dot(Normal,Light) > 0.0) {
    vec3 ref = normalize(2.0 * Normal * dot(Normal,Light) - Light);
    s = pow(max(dot(Eye,ref), 0.0), Shininess);

  }

  vec4 specular = uSpecular * s * vec4(SpecularColor,1.0) * lightAlpha;

  return vec4(ambient.rgb + diffuse.rgb + specular.rgb, 1.0);
}


// is within the volume
bool isWithinVolume(vec3 p) {
	// determine that this point lies above and below the x,y,z positions that define this shape
	/**/
	return (
		// within x coordinate
		p.x >= vVolumeStart.x && p.x <= (vVolumeStart.x + vVolumeDimens.x) &&
		// within y coordinate
		p.y >= vVolumeStart.y && p.y <= (vVolumeStart.y + vVolumeDimens.y) &&
		// within z coordinate
		p.z >= vVolumeStart.z && p.z <= (vVolumeStart.z + vVolumeDimens.z)
	);
	/**/
	//return true;

}


// Fixed probe, the data being viewed is fixed within the volume in model coordinates
// This can be changed by rotations and such, but only on the object itself
// gets the color using a ray cast, without a definite volume boundary
// for model coordinates (local)
void getColor_ByRayCast_NoBound_UsingModelCoordinates() {

	// changes the ray step size
	// 0.025, 0.0125
	float rayStepSizeMultiplier = 0.1;
	// max # of steps that are allowed before stopping the ray cast
	int maxSteps = 30;

	// default is no color
	vec4 color = vec4(0.0);

	// calculate small ray that steps through at fixed increments
	// TODO what's wrong with this?
	vec3 rayStep = (normalize(vECPosition) * rayStepSizeMultiplier).xyz;

	// progressive rayCast that steps through the volume
	vec3 rayCastPos = vMCPosition;

	// counts ray cast steps, used to exit out
	int steps = 0;

	// isWithinVolume(rayCastPos)
	while(isWithinVolume(rayCastPos)) {

		if(color.a >= 1.0) {
			// color maxed out, exit
			break;
		}

		if(steps > maxSteps) {
			break;
		}

		// circle test
		/*
		// x^2 + y^2 + z^2 = r^2 (should be <= a certain radius to be within the sphere)
		if(sqrt(pow(rayCastPos.x, 2) + pow(rayCastPos.y, 2) + pow(rayCastPos.z, 2)) <= 0.52) {
			// shade in white
			color = vec4(1.0);
			break;

		}
		/**/

		/**/
		// get fractal brownian motion val
		float f = fbm(rayCastPos.xyz + uSeed) * 2.0;
		//float f = fbm(rayCastPos.xyz + fbm(rayCastPos.xyz + fbm(rayCastPos.xyz + uSlowTime + uSeed))) * 2.0;
		// only apply if greater than 0.8
		if(f > 0.8) {
			// add color only if exceeds the threshold to display
			vec4 tColor = mix(
				vec4(0.0),
				vec4(1.0),
				(f - 0.8)
			);
			tColor *= 1.0;
			color += tColor;

		}
		/**/

		// step ray through the volume
		rayCastPos += rayStep;

		steps++;
	}


	/*
	if(color.a == 0.0) {
		// discard if nothing to display
		discard;

	}
	/**/

  // hold alpha
  float alpha = 1.0;

	// to give effect of looking at water
	//color.r = 0.0;

  // calc fragment lighting with color
  color = perFragmentLighting(color);

	// aply fragment color
  gl_FragColor = vec4(color.rgb, alpha);

}



// World Coordinate Probe, useful for scouring data across large sections of space potentially
// gets the color using a ray cast, without a definite volume boundary
// for eye coordinates (camera based, makes this a probe)
void getColor_ByRayCast_NoBound_UsingEyeCoordinates() {

	// changes the ray step size
	// 0.025, 0.0125
	float rayStepSizeMultiplier = 0.01;
	// max # of steps that are allowed before stopping the ray cast
	int maxSteps = 200;

	// default is no color
	vec4 color = vec4(0.0);

	// calculate small ray that steps through at fixed increments
	vec3 rayStep = (normalize(vECPosition) * rayStepSizeMultiplier).xyz;

	// progressive rayCast that steps through the volume
	vec4 rayCastPos = vECPosition;

	// counts ray cast steps, used to exit out
	int steps = 0;

	// convert point to model coordinates before we start
	vec4 convertedPoint = vModelViewMatrix_Inverse * rayCastPos;

	// TODO old ray tracing method for lighting
	// setup 2 other converted ray casted points to trace through volume
	// the end result of the 3 converted points will be used
	// to compute the normal via the cross product of the 2 resultant vectors
	//vec4 rc2,rc3;
	//vec4 c2,c3;
	//rc2 = vec4(rayCastPos.x+0.01, rayCastPos.y, rayCastPos.z, rayCastPos.w);
	//rc3 = vec4(rayCastPos.x, rayCastPos.y, rayCastPos.z+0.01, rayCastPos.w);
	// setup default colors for these as well
	//c2 = c3 = vec4(0.0);

	// isWithinVolume(rayCastPos)
	// isWithinVolume(convertedPoint.xyz)
	while(isWithinVolume(convertedPoint.xyz) || steps == 0) {
		if(color.a >= 1.0) {
			// color maxed out, exit
			break;
		}

		if(steps > maxSteps) {
			break;
		}

		/**/
		// get fractal brownian motion val
		float f = fbm(convertedPoint.xyz + (uSlowTime * 0.5) + uSeed) * 2.0;
		//float f = fbm(convertedPoint.xyz + fbm(convertedPoint.xyz + fbm(convertedPoint.xyz + uSlowTime * 0.5 + uSeed))) * 2.0;
		// only apply if greater than 0.8
		if(f > 0.8) {
			// add color only if exceeds the threshold to display
			vec4 tColor = mix(
				vec4(0.0),
				vec4(1.0),
				(f - 0.8)
			);
			tColor *= 0.222;
			color += tColor;

		}

		// step ray through the volume
		rayCastPos.xyz += rayStep;

		// recalc next converted point
		convertedPoint = vModelViewMatrix_Inverse * rayCastPos;

		// Do again for Ray Cast #2
		/*
		if(c2.a < 1.0) {
			vec4 cp2 = vModelViewMatrix_Inverse * rc2;
			f = fbm(cp2.xyz + (uSlowTime * 0.5) + uSeed) * 2.0;
			if(f > 0.8) {
				vec4 tColor = mix(
					vec4(0.0),
					vec4(1.0),
					(f - 0.8)
				);
				tColor *= 0.222;
				c2 += tColor;
			}
			rc2.xyz += rayStep;
		}
		*/

		// Do again for Ray Cast #3
		/*
		if(c3.a < 1.0) {
			vec4 cp3 = vModelViewMatrix_Inverse * rc3;
			f = fbm(cp3.xyz + (uSlowTime * 0.5) + uSeed) * 2.0;
			if(f > 0.8) {
				vec4 tColor = mix(
					vec4(0.0),
					vec4(1.0),
					(f - 0.8)
				);
				tColor *= 0.222;
				c3 += tColor;
			}
			rc3.xyz += rayStep;
		}
		*/

		// bump our step count
		steps++;

	}

	// color red if we are exceeding the ray length
	// this is a debug feature
	if(steps > maxSteps) {
		color = vec4(1.0, 0.0, 0.0, 1.0);

	}


	if(color.a == 0.0) {
		// discard if nothing to display
		// no need to draw empty fragments
		discard;

	}

	// compute cross product of resulting ray cast end positions
	//vec3 v1 = (rayCastPos - rc2).xyz;
	//vec3 v2 = (rayCastPos - rc3).xyz;

	// calculate vector from final point rayCastPos to vECLight
	// reduce to unit vector
	// reduce to 1/10th of the unit vector
	vec3 vRL = normalize(rayCastPos.xyz - vECLight) * rayStepSizeMultiplier;
	// apply fixed increments until we exit the volume
	rayCastPos.xyz += vRL;
	convertedPoint = vModelViewMatrix_Inverse * rayCastPos;
	steps = 0;
	while(isWithinVolume(convertedPoint.xyz)) {
		if(lightAlpha >= 1.0) {
			break;
		}

		if(steps > maxSteps) {
			break;
		}

		// sum up the alphas using the same equation as above
		float g = fbm(convertedPoint.xyz + (uSlowTime * 0.5) + uSeed) * 2.0;
		if(g > 0.8) {
			float tAlpha = mix(
				0.0,
				1.0,
				(g - 0.8)
			) * 0.222;
			lightAlpha += tAlpha;
		}

		rayCastPos.xyz += vRL;
		convertedPoint = vModelViewMatrix_Inverse * rayCastPos;
		steps++;

	}

	// use the inverse of the alpha to apply a lighting change
	lightAlpha = 1.0 - lightAlpha;

	// use vector from ray to light for normal
	//ReplaceNormal = normalize(cross(v1,v2) + vRL);
	ReplaceNormal = normalize(vECLight - rayCastPos.xyz);

  // hold alpha
	float alpha = color.a;

  // calc fragment lighting with color
  color = perFragmentLighting(color);

	// aply fragment color
  gl_FragColor = vec4(color.rgb, alpha);

}


void main() {

	// this one does not help
	//getColor_ByRayCast_NoBound_UsingModelCoordinates();

	// this is the one to use
	getColor_ByRayCast_NoBound_UsingEyeCoordinates();

}
