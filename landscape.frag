//#version 210 compatibility

uniform float	uTime;		// "Time", from Animate( )
uniform float slowTime; // Slow Time from Animate()
uniform float slowSlowTime; // slowest time

//uniform int activeWarpColor; // texture warp active
varying vec2  	vST;		// texture coords
varying vec3    vMCPosition;

// shaded normal, light, eye vectors
varying vec3 Ns;
varying vec3 Ls;
varying vec3 Es;

// holds the random seed we will be using
uniform float uRandVal1;
//uniform float uRandVal2;

// GRADIENTS (5 alters)
uniform vec3 uG0;
uniform vec3 uG1;
uniform vec3 uG2;
uniform float uG3;

// from S.O., what does dot() do? (dot product, yes), and fract() (fraction, yes)
float rand(vec2 co, float seed){
    return fract(sin(dot(co.xy * seed, vec2(12.9898,78.233))) * 43758.5453);
}

float rand(float x, float seed){
    return fract(sin(dot(vec2(x,seed), vec2(12.9898,78.233))) * 43758.5453);
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

/*
 * Flow 1 component
 */
vec3 flow1() {
  float r=0.0,g=0.0,b=0.0;

  /* cubic noise? */
  // trig with respect to time
  float ts = cos(uTime * 2.0 * 3.14159);
  float cs = cos((vST.t + uTime) * 2.0 * 3.14159);
  float ss = sin((vST.s + uTime) * 2.0 * 3.14159);
  float slowCs = sin((vST.t + slowTime) * 2.0 * 3.14159);
  float slowSs = sin((vST.s + slowTime) * 2.0 * 3.14159);

  //
  //
  /// RED FLOW (uses trig on vST.t & vST.s, but shifts actual values along by uTime from 00-1.0, to get shift)
  //
  //
  // create one function to change along
  float s = (3.0 * pow(cs, 2)) - (2.0 * pow(ss, 3)) - (4.0 * pow(slowCs, 4)) + ts;

  // create another function at a diff power to change along
  float t = (10.0 * pow(ss, 3)) - (15.0 * pow(cs, 4 )) + (6.0 * pow(slowSs, 5)) + ts;

  float z1 = cos(vST.s * 4.0);
  float z2 = sin(vST.t * 4.0);

  // calculate red value with both functions overlayed & multiplied

  // simple
  //r = s * t;

  // more complex
  //r = s * z1 + t * z2;

  // psychedelic waves
  //r = pow(cos(s),3) + pow(sin(t),3);

  // all 3 mixed
  r = cos(sqrt((s * t) + (s * z1 + t * z2) + (pow(cos(s),3) + pow(sin(t),3)) * s * t + slowCs));

  //g+=((sin(uTime * 6.0 * 3.14159)+1.0));
  //b+=((cos(uTime * 6.0 * 3.14159)+1.0));

  // add green for yellow trim at partial edge
  if(r > 1.0) {
    g = (r - 0.7) * 0.05;
  }

  // discard for holy effect
  if(r > 0.5) {
    discard;
  }

  // add in blue as time goes on
  if(r > 1.0) {
    b = ts;
  }

  return vec3(r,g,b);

}

/* sucks... */
vec3 cubic_flow() {

  float r = 0.0;

  float ts = cos(uTime * 2.0 * 3.14159);

  float csp = cos((vST.t + uTime) * 2.0 * 3.14159);
  float csn = cos((vST.t - uTime) * 2.0 * 3.14159);

  float ss = sin((vST.s - uTime) * 2.0 * 3.14159);
  float slowCs = sin((vST.t + slowTime) * 2.0 * 3.14159);
  float slowSs = sin((vST.s) * 2.0 * 3.14159);

  float f1 = (3.0 * pow(csp, 2)) - (2.0 * pow(csp, 3));
  float f2 = (5.0 * pow(csn, 2)) - (7.0 * pow(csn, 3));

  float s1 = (3.0 * pow(slowSs, 2)) - (6.0 * pow(slowSs, 3));

  r = f1 * f2 * cos(vST.s * 2.0 * 3.14159) * sin(vST.t * 2.0 * 3.14159);

  return vec3(r,0.0,0.0);
}


// use these cubic random values to run the LCG off of
//#define CUBIC_NOISE_RAND_A 134775813
//#define CUBIC_NOISE_RAND_B 1103515245

// calculate cubic noise here
/*
float cubicNoiseRandom(int seed, int x, int y) {
	return
		((
      pow(((pow(x * 1.0,y)) * CUBIC_NOISE_RAND_A) * 1.0,(seed + x))) *
		(((CUBIC_NOISE_RAND_B * x) << 16) ^ (CUBIC_NOISE_RAND_B * y) - CUBIC_NOISE_RAND_A)) / UINT32_MAX;
}
*/

// simple random cubic noise
/**/
float cubicNoiseRandom(int seed, int x, int y) {
  return rand(vec2(x,y),float(seed)) * 100.0;
}

int mod(int x, int y) {
  int rez = x - (y * int(x/y));
  if(rez >= 0) {
    return rez;
  } else {
    return rez * -1;
  }
}

// generate tile for cubic noise
int cubicNoiseTile(int coordinate, int period) {
	//return mod(coordinate,period);
  return mod(coordinate, period);
  //return coordinate;
  //return coordinate - period * int(floor(float(coordinate)/float(period)));
}

// interpolate cubic noise from this point
float cubicNoiseInterpolate(float a, float b, float c, float d, float x) {
	float p = (d - c) - (a - b);
  // there's the cubic...
  // p*x^3 + ((a-b) - p)*x^2 + + (c - a)*x + b
	return x * (x * (x * p + ((a - b) - p)) + (c - a)) + b;
}


// generates a cubic noise sample from 2D information
float cubicNoiseSample2D(float x, float y, int octave, int periodx, int periody)
{
  // some fixed seed to alter the rand by
  int seed = 2;

  // calculate periods with respect to the octave
  int periodX = periodx / octave;
  int periodY = periody / octave;

  // calculate xi & yi with respect to the octave
  int xi      = int(floor(x / float(octave)));
	float lerpx = x / float(octave) - float(xi);
	int yi      = int(floor(y / float(octave)));
	float lerpy = y / float(octave) - float(yi);

  // set of all samples
	float samples[4];

  return float(yi);

  /*
  if(cubicNoiseTile(xi, periodX) != 0) {
    return float(mod(yi,periodY));
  } else {
    return float(cubicNoiseTile(xi, periodX));
  }
  */

  //return float(mod(yi,2));

  //return float(cubicNoiseTile(xi, periodX))mod(yi,periodY);

	for(int i = 0; i < 4; i++) {
		samples[i] = cubicNoiseInterpolate(
			cubicNoiseRandom(
        seed,
				cubicNoiseTile(xi - 1,      periodX),
				cubicNoiseTile(yi - 1 + i,  periodY)
      ),
			cubicNoiseRandom(
        seed,
				cubicNoiseTile(xi,          periodX),
				cubicNoiseTile(yi - 1 + i,  periodY)
      ),
			cubicNoiseRandom(
        seed,
				cubicNoiseTile(xi + 1,      periodX),
			  cubicNoiseTile(yi - 1 + i,  periodY)
      ),
			cubicNoiseRandom(
        seed,
				cubicNoiseTile(xi + 2,      periodX),
				cubicNoiseTile(yi - 1 + i,  periodY)
      ),
			lerpx
    );
  }

  //return samples[0];
  //return samples[0] + samples[1] + samples[2] + samples[3];

	return cubicNoiseInterpolate(samples[0], samples[1], samples[2], samples[3], lerpy) * 0.5 + 0.25;
}
/**/

float convert_to_tile(float i) {
  // shaves off everything past the 10's place in decimal, giving us a nearby grid point
  return floor(i * 10.0) / 10.0;
}

float greenThing = 0.0;

// TODO get cubic noise working...
float get_cubic_noise(float x, float y) {
  float seed = 3.0;

  // calculate offset points
  float offset = 0.5;
  float tx1 = convert_to_tile(x - offset);
  float tx2 = convert_to_tile(x + offset);
  float ty1 = convert_to_tile(y - offset);
  float ty2 = convert_to_tile(y + offset);

  // calc the value at each block point
  float rx1,rx2,ry1,ry2;

  // compute random values for each tile pair
  //  TODO PICK UP FROM HERE
  // TODO changed from computing for each slat going up & down (L/R x and Top/Bot y)
  // TODO if we go this way the code below needs to be changed to factor in the location between points (i.e., distance computation is wrong below)
  // This seems like it should resolve the issues with the banding, and we'll get decent noise, get this bilinear version working later
  rx1 = rand(vec2(tx1,ty1), seed);
  rx2 = rand(vec2(tx1,ty2), seed);
  ry1 = rand(vec2(tx2,ty1), seed);
  ry2 = rand(vec2(tx2,ty2), seed);

  // calculate linear interpolation for x
  // 1. calc total distance between tile points
  float xd = abs(tx2 - tx1);
  // 2. calc distance between x and tile x1
  float rd1 = abs(x - tx1);
  // 3. calc ratio from x and tile x1
  // TODO ratio is GOOD
  float ratio = rd1 / xd;
  // 4. weight rx1 to % x & x1

  // TODO Problem below, ratio AND rx1 are both strips of 10
  // Good part: rx1 should be a strip, we want constant rands
  // Bad Part: ratio should NOT be a strip, we should have continuity

  rx1 = rx1 * ratio;
  // 5. weight rx2 to % x & x2
  rx2 = rx2 * (1.0 - ratio);
  // 6. calc lerp as rx1 + rx2
  float lerpx = ratio;

  //float lerpx = (rx1 * (1.0 - (x - tx1)/(tx2 - tx1))) + (rx2 * ((tx2 - x)/(tx2 - tx1))); // non-simplified equation
  //float lerpx = (rx1 * (1.0 - (x - tx1)/(tx2 - tx1))) + (rx2 * ((x - tx2)/(tx2 - tx1)));

  // calculate linear interpolation for y
  float lerpy = (ry1 * (1.0 - (abs(y - ty1))/(ty2 - ty1))) + (ry2 * ((abs(y - ty2))/(ty2 - ty1)));

  float lerp = lerpx;

  //float f2 = (5.0 * pow(lerp, 3)) - (7.0 * pow(lerp, 2) + (2.0 * pow(lerp,1)) + 0.1);
  //return rand(vec2(x,y),seed);

  return lerp;
}


// FINAL cubic noise implementation
float cubic_noise_final(float t, float freq, float mag, float seed) {

  // apply any increased frequency
  t = t * freq;

  int ogT = int(t * 5.0);

  t = (t * 5.0) - float(ogT);

  // cubic noise
  /*
  float cn0 = 1.0 - 3.0*t*t + 2.0*t*t*t;
  float cg0 = t - 2.0*t*t + t*t*t;
  float cg1 = -1.0*t*t + t*t*t;
  float cc0 = 0.0;
  float cc1 = 0.0;
  /**/

  // quintic noise
  /**/
  float cn0 = 1.0 - 10.0*t*t*t + 15.0*t*t*t*t - 6.0*t*t*t*t*t;
  float cg0 = t - 6.0*t*t*t + 8.0*t*t*t*t - 3.0*t*t*t*t*t;
  float cg1 = -4.0*t*t*t + 7.0*t*t*t*t - 3.0*t*t*t*t*t;
  float cc0 = 0.5*t*t - 1.5*t*t*t + 1.5*t*t*t*t - 0.5*t*t*t*t*t;
  float cc1 = 0.5*t*t*t - t*t*t*t + 0.5*t*t*t*t*t;
  /**/

  float cn1 = 1.0 - cn0;

  // create nodes, always at zippo
  float n0 = 0.0;
  float n1 = 0.0;

  // gradients
  float g0;// 0.4
  float g1;// 0.4

  if(ogT == 0) {
    // 1st band
    g0 = uG0[0];
    g1 = uG0[1];

  } else if(ogT == 1) {
    // 2nd band
    g0 = uG0[2];
    g1 = uG1[0];

  } else if(ogT == 2) {
    // 3rd band
    g0 = uG1[1];
    g1 = uG1[2];

  } else if(ogT == 3) {
    // 4th band
    g0 = uG2[0];
    g1 = uG2[1];

  } else {
    // 5th band (final)
    g0 = uG2[2];
    g1 = uG3;

  }

  //curvatures
  float c0 = rand(float(ogT), seed * 2.0) * 0.4;
  float c1 = rand(float(ogT), seed) * 0.4;

  // noise out of slide
  float noise = cn0*n0 + cn1*n1 + cg0*g0 + cg1*g1 + cc0*c0 + cc1*c1;

  // apply any amplitude magnification
  return noise * mag;
}


// gets some Perlin noise for us
float GetNoise(vec2 v, int iterations, float seed) {
  // order, which will double per iteration
  // used to double frequency, and halve magnitude, per iteration

  // apply via S
  int iOg = iterations;
  float order = 1.0;
  float noise = 0.0;
  while(iterations-- > 0) {
    noise = noise + cubic_noise_final((v.s * 0.1), order, 1.0 / order, seed);
    order = order * 2.0;
  }


  // apply via T
  iterations = iOg;
  order = 1.0;
  float noise2 = 0.0;
  while(iterations-- > 0) {
    noise2 = noise2 + cubic_noise_final((v.t * 0.2), order, 1.0 / order, seed);
    order = order * 2.0;
  }

  // apply via sqrt S
  iterations = iOg;
  order = 1.0;
  float noise3 = 0.0;
  while(iterations-- > 0) {
    noise3 = noise3 + cubic_noise_final((sqrt(v.s * 0.4)), order, 1.0 / order, seed);
    order = order * 2.0;
  }

  // apply via pow T
  iterations = iOg;
  order = 1.0;
  float noise4 = 0.0;
  while(iterations-- > 0) {
    noise4 = noise4 + cubic_noise_final((pow(v.t*0.8,2.1)), order, 1.0 / order, seed);
    order = order * 2.0;
  }

  return (noise+noise2+noise3+noise4);

}


// calc the ripples and such
float colorCircles(vec2 v, float noise) {

  float diam   = 0.25;
  float radius = diam / 2.0;

  float ns = float(int(v.s/diam));
  float nt = float(int(v.t/diam));

  float sc = ns * diam + radius;
  float tc = nt * diam + radius;

  // 1. centers
  float ds = v.s - sc;
  float dt = v.t - tc;

  // calc old distance
  float oldDist = sqrt(ds*ds + dt*dt);

  float newDist = oldDist + noise;

  // create scale using noise
  float scale = newDist / oldDist;

  ds *= scale;
  dt *= scale;
  float circle = sqrt(ds*ds + dt*dt);

  //float circle = sqrt(pow(ds, 2.0) + pow(dt, 2.0));

  if(circle < radius) {
    return 1.0;
  } else {
    return 0.0;
  }
}


void main() {

  // Cubic noise attempt
  // starting red color
  float r = 1.0;

  float noise = GetNoise(vST + uTime * 2.0, 4, uRandVal1);

  float rng = rand(vST, 401184108.0);

  vec3 color = vec3(rng,rng,rng);

  // experiment
  //r = cos(r * 2.0 * 3.14159);
  //float g = cos(vST.s*vST.s*slowTime) - sin(vST.t*vST.t*uTime*20.0);
  //vec3 color = vec3(noise,0.0,0.0);


  // experiment #2
  // working with circles
  //vec3 color = vec3(colorCircles(vST, noise), 0.0, 0.);



  // flow 1 approach
  // Probably the best I have come up with so far, looks like ripples...lots of sin/cos waves being applied additively
  //vec3 color = flow1();
  //float n = color.r;

  // 'fake' cubic flow
  //vec3 color = cubic_flow();

  // actual cubic flow
  // TODO not quite cubic noise
  // x & y, octave, periodx & periody
  //float n = cubicNoiseSample2D(vST.s * 100.0, vST.t * 100.0, 4, 40, 40);
  //vec3 color = vec3(n,n,n);

  /*
  vec3 color;
  color.r = get_cubic_noise(vST.s, vST.t);
  //color.r = get_cubic_noise(vST.s + cos(uTime * 2.0 * 3.141589), vST.t + sin(uTime * 2.0 * 3.141589));
  color.g = greenThing;
  color.b = 0.0;
  /**/

  /* TODO, this is a good discard, more or less...
  if(n < (cos(uTime * 2.0 * 3.14159) + 1.0) / 3.0 + 0.25) {
    discard;
  }
  /**/

  // apply fragment color
  gl_FragColor = vec4(vMCPosition.y, vMCPosition.x, 0.0, 1.0);


}
