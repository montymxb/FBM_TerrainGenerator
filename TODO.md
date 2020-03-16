:: Plan for the CS 557 Final Project ::
- setup a large quadrilateral for the base
  - will do water, and land
- setup something for the sky? point cloud maybe

- Windows setup
  - with GLMan, can auto gen basic layouts quickly
  - entropy comes GLMan rand...somewhat okay
- Mac setup (more work but it's what I want)
  - with OpenGL, I have to setup the quad by hand
  - then fragment shader will work as expected
  - entropy comes from whatever we setup outside..., and we have control over that entropy
  - don't have to lug around the external-drive

- Fallbacks in case you have problems generating terrain (water should be okay)
  1. Terrain should be loaded via noised texture
  2. Use GLMan with same shaders, and utilize noise texture to generate better land stuff




:: TODO ::
- cleanup the final project, isolating the large quad code we made (retain experimental shader code, cite attribution if you use it!)
~ move as much graphics logic into Special OpenGLMB class
- move camera on top of the quad
- scale up the quad
- implement some basic random height map in vertex shader, verify we have enough density to make land
- make a dense quad that is actually a square...
- verify base fragment shader coloring works
  - implement color by height
- implement per-vertex lighting (pull from last assignments loaded in flip)
- implement a continuous noise function in Vertex Shader
  - use this to generate a basic heightmap
- smooth the heightmap using the mix step function, as in it should smooth out on the edges
- in fragment shader color red above a point & blue below a point in the graph
- perturb normals based on wave function to give basic water animation (try later)
- Add step function to gradually shift color for hills until it maxes out at white for the top
- Implement a single sheet above, color with faded coloring to look like clouds
- improve terrain generation with modified fbm
- improve terrain lighting by adjusting normal based on surface position
- improve terrain coloring by using a series of mixing
  - mix green to brown to white
  - green -> brown 0.0 - 0.3
  - brown -> dark brown 0.3 - 0.8
  - dark brown -> white 0.8 - 1.0
- improve water quality by applying fbm there as well, with light coloring
  - as separate sheet
- set water with high specular highlight
- mess with normals on water (will give concept of waves..)
- wasn't able to discern how to make 'breaking' waves, which would have been nice
- try to make white water in a 0.01 color mix
- improve cloud quality by coloring with respect to S and T (make them super smooth okay!)
- improve cloud quality by applying vertex displacement as well
  - increase density of sheet or scale it up (and clouds) by quite a lot
- also perturb normals on this to make it look like the water, but slightly different, with clouds
  - compute normal by calculating 2 points slightly off, and then compute cross product to get normal
- Polish this and see how it looks (fucking amazing)
- If time consider a volume for cloud
- If time consider physical wave displacement as a 3rd sheet that intersects the land, and is separate
  - means discarding the other section of the land as we work
- try to use the same cloud logic code to add shadows in the land
  - then we're done!

- done, write up the report now


# references
- https://thebookofshaders.com/13/
- https://www.youtube.com/watch?v=XXSCwm06J_w
- https://www.shadertoy.com/view/Ms2SD1
- https://www.shadertoy.com/view/MdX3zr
