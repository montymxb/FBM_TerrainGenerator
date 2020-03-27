# FBM_TerrainGenerator
Fractal Brownian Motion Terrain Generator. Produces land, water, and sky components. Demonstrational. This includes code for a flat sheet of clouds, and more recently cloud volumes (as can be seen below).

<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/cloud2.gif">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/cloud3.gif">

Using shaders exclusively, feeds in results from a hash function to create a reasonable distribution of values to then feed into fbm.
Although not truly random, the results are fairly good. The hash function itself isn't of great quality, but it allows this kind of terrain generation to be independent of feeding texture data into glsl, and to that end does a pretty good job.

<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/p1.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/water.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/landwater.png">

Based on the examples and details provided in [the book of shaders](https://thebookofshaders.com/13/), by Patricio Gonzalez Vivo and Jen Lowe (an excellent resource).

Additionally based on the concepts for terrain generation provided in [Job Talle's post on cubic noise](https://jobtalle.com/cubic_noise.html).


<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/smallcloud.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/p5.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/p6.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/p7.png">

Additionally, I added volumetric ray casting via shaders to render clouds as volumes instead of sheets. The definition can be variably adjusted to increase the quality of the clouds. These pictures were taken on my machine, which doesn't handle this too great, so the quality is a bit low, but the progression of the ray casting can be seen in the volumetric rendering.

<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/cloud1.gif">

The lighting is something that could still be improved. Currently, it's taking the end point of the ray trace used to compute the color at a given point looking into the volume. Wherever the ray may have ended, or exited, the lighting is reduced by the number of additional values between that point and the source of the light. This is another ray cast towards the light source, and this reduces the effective lighting by any additional values it encounters.

<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/b1.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/b2.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/b3.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/b4.png">
