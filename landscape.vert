//#version 210

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

// approximate eye light position
vec3 eyeLightPosition = vec3(LightX,LightY,LightZ);

const float PI = 	3.14159265;
const float AMP = 	0.2;		// amplitude
const float W = 	2.;		// frequency

// from S.O., what does dot() do? (dot product, yes), and fract() (fraction, yes)
float rand(vec2 co, float seed){
    return fract(sin(dot(co.st * seed, vec2(12.9898,78.233))) * 43758.5453);
}

float rand(float x, float seed){
    return fract(sin(dot(vec2(x,seed), vec2(12.9898,78.233))) * 43758.5453);
}


void perFragmentLighting(vec4 ECPosition) {
	vec3 normal = normalize(gl_NormalMatrix * gl_Normal);

	// store model coordinates for use in frag shader
	vMCPosition = gl_Vertex.xyz;

	// set surface normal
	Ns = normal;

	// calc vecotr from point to light
	Ls = eyeLightPosition - ECPosition.xyz;

	// vec from point to eye position
	Es = vec3(0.0, 0.0, 0.0) - ECPosition.xyz;

	// adjust existing normals
	/*
	Ns = normal * Ns;
	Ls = normal * Ls;
	Es = normal * Es;
	*/

}


void main() {

	vec4 ECPosition = gl_ModelViewMatrix * gl_Vertex;

	perFragmentLighting(ECPosition);

	vST = gl_MultiTexCoord0.st;
	//vec3 vert = ECPosition.xyz;
	vec3 vert = gl_Vertex.xyz;

  //if(activeWarpCords == 1) {
	/*
    // alter along X with a sin wave
    if(abs(vert.x) > 2.0) {
      vert.x = vert.x * (sin(uTime * 2.0 * PI) + 2.0);
    }

    // calculate how y would be altered with a sin wave
    float sinY = vert.y * sin(uTime * 2.0 * PI);

    // alter along Y with regard to how Z would be transformed by a cos wave
    vert.y = vert.y + (vert.z * cos(uTime * 2.0 * PI));

    // alter X & Y with application of sinY
    vert.x = vert.x + sinY;
    vert.z = vert.z + sinY;
		*/

  //}

	//vert.y = (vert.y * sin(uTime * 2.0 * PI + vert.y));
	//vert.y = (rand(vST, 2.0) * 0.1 * sin(uTime * 2.0 * PI + vert.x * vert.y));
	vert.y = sin(uTime * 2.0 * PI + vST.s + vST.t);

	vMCPosition = vert.xyz;

	gl_Position = gl_ModelViewProjectionMatrix * vec4(vert,1.0);

}
