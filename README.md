# FBM_TerrrainGenerator
Fractal Brownian Motion Terrain Generator. Produces land, water, and sky components. Demonstrational.

Using shaders exclusively, feeds in results from a hash function to create a reasonable distribution of values to then feed into fbm.
Although not truly random, the results are fairly good. The hash function itself isn't of great quality, but it allows this kind of terrain generation to be independent of feeding texture data into glsl, and to that end does a pretty good job.

Based on the examples and details provided in [the book of shaders](https://thebookofshaders.com/13/), by Patricio Gonzalez Vivo and Jen Lowe (and excellent resource).

Additionally based on the concepts for terrain generation provided in [Job Talle's post on cubic noise](https://jobtalle.com/cubic_noise.html).

<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/p1.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/p2.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/p3.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/p4.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/p5.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/p6.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/p7.png">

Additionally, I added volumetric ray casting via shaders to render clouds as volumes instead of sheets. The definition can be variable adjusted to increase the quality of the clouds. These pictures were taken on my machine, which doesn't handle this too great, so the quality is a bit low, but the progression of the ray casting can be seen in the volumetric rendering.

<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/cv1.png">
<img width=500 src="https://github.com/montymxb/FBM_TerrrainGenerator/blob/master/submission/cv2.png">
