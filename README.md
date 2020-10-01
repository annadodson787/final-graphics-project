# COSC 77 Final Project Report

### Benjamin Coleman, Anna Dodson

### October 1, 2020

## Image Overview

Our final project seeks to render a realistic deep-ocean scene using techniques including Per-
lin noise landscape generation, caustic water mapping, object rendering with transparent
textures, skybox, and fog effects.


## Implementation Details

This section presents specific math models, algorithms, and code implementations which
informed our major design decisions.

### Perlin Noise Terrain Generation

The first step in rendering a realistic ocean scene was to create the ocean floor. We wanted
to implement terrain generation and found that Perlin noise would be suitable for generating
smooth terrain; however, the frequencies and scales
The Perlin noise function is applied to a planar triangle mesh, which we originally placed at
position -10 below the camera and mipmapped a sandy image texture onto. The Perlin noise
is then applied to the height (y) of each vertex in the mesh. It is generated using a seed
and is dependent on the x and z position of each vertex in this initial planar mesh; however,
to generate a smoother surface, we used an offset variable in both the x- and z- position, so
that the gradient changes very slowly over time. Finally, the height is scaled up.

We added Perlin noise at multiple frequencies to generate both larger and smaller textures
in the terrain. The parameters which we found to work best were:

- 30 ∗(pn.noise(xof f, zof f,. 5 f))− 15
- 20 ∗(pn.noise(x, z,. 99 f))− 10

Note that while the first texture is dependent on the offset x and z, the latter is dependent
on raw x and z, which scale much faster. To account for this, the third parameter to the 3D
Perlin Noise function is set very close to 1, so that the overall distance will be less hectic.
The final height is set to be the sum of the two frequencies, which enables smaller Perlin
bumps.

The terrain was rendered using Lambertian shading; this was already in place for some of
the other textures, so we didn’t have to change anything. However, we did have to specify
where in the sand texture each u and v would occur (on the scale [0, 1]) for each vertex in
the terrain. We decided to use simply the x, z position to determine the texture sampling;
since the landscape was relatively gradually sloped, this produced a satisfactory visual effect.


### Caustic Mapping

Caustic mapping was the main technical challenge of our project. It is done in a similar
manner to shadow mapping, whereby there are two passes. The first pass creates an image,
and the second applies the image to the objects in the scene.

For the image generation step, we assume there is a fixed light source and compute an image
projection onto a flat plane through some refractive layers. We apply Snell’s law to generate
the images given an assumed mesh based on a wave theory model (this uses sinusoidal mod-
els to generate realistic circular wave-like layers at several different frequencies, which when
added together, create the resulting images). The technique used varies in time and space,
hence earning the name ”periodic caustic mapping” (http://www.dgp.toronto.edu/people
/stam/suomi/stam/caustics.html).

The images generated from this technique look like this:

For the second pass, we want to render the textures created from the caustic generation step
onto the objects in the scene and use them as a lighting source that appears natural. We


```
can then apply them in a time-varying way by updating the caustics passed to the fragment
shader.
```
```
The first step was to get the textures mapped onto the scene. In order to do this, we pass
the caustics as a sampler2D object to the fragment shader and read in the texture, passing
it to whatever lighting effect is already in place. The sampling on the caustic effect had to
be computed for each position in the scene; this was done through shared.vert as a modulus
effect, so that the pattern would be evenly applied across the entire ocean floor and all ob-
jects. The caustic lighting effect is then multiplied to the effect of the previous lighting for
each object, not added, because we want the caustic to darken the image where there are
shadows and lighten it where there are not. The effect was also altered slightly to appear a
bit bluish in both the shadow regions and the light regions by modifying the blue channel of
the light by a constant factor, effectively rendering a deep-ocean effect.
```
1 // normalizing uv f o r generative texture patterning
2 vtx normalized uv = vec2 (( pos. x) /500. f , ( pos. z ) /500. f ) ;
3
4 f l o a t samplerx = pos. x− int (( pos. x) /100)∗ 100 + normal. x ;
5 f l o a t samplery = pos. z − int (( pos. z ) /100)∗ 100 + normal. z ;
6 vtx caustic uv = vec2 ( sampler x /10. f , samplery /20. f ) ;

1 // modify the l i g h t color based on the computed cau sti c color
2 vec3 lightCol = l t [ i ]. d i f. rgb ∗ vec3 ( c aus tic. r , c aus tic. g + .05 , ca ust ic. b +

. 1 ) ;

```
Notice how the existing lighting effect persists; there is still a contour effect on the areas
which would be partially occluded or shaded from the lights in the scene. Additionally, the
texture had to be applied multiple times, since the landscape space was much larger than
the caustic images we generated.
```
```
The original caustic images were quite sharp, appearing as though they might be refractions
```

```
on the bottom of a swimming pool. In order to simulate the murkiness of ocean water, we
sampled from multiple caustic images at once. All in all, there were 5 caustic sampler2Ds
passed to the fragment shaders. This was also helpful in aiding with the animation transi-
tions between images, so they did not appear sticky or slow.
```
1 vec4 causticColor = .4∗texture ( caustic , vtx caustic uv ) +.3∗texture ( caustic2 ,
vtx caustic uv ) +.2∗texture ( caustic3 , vtx caustic uv ) +.1∗texture ( caustic4 ,
vtx caustic uv ) +.05∗texture ( caustic5 , vtx caustic uv ) ;

```
As mentioned above, by passing the caustic maps to the shared vertex shader we were able
to render the caustic effects on all objects, not just the sea floor. One excellent side effect of
this is the appearance of ”Gods Rays” on the cliff wall.
```
### Animation with Caustic Mapping

```
The final part of implementing caustic mapping was rendering a realistic animation sequence
for the shadows on the surfaces of the objects. In order to implement this, we loaded in 16
different images based on the temporal caustic function explained above. However, rendering
each of the images one after another appeared very choppy. Therefore, we had to apply a
blended solution.
```
```
To ease the transition between frames, we scaled the time vector down and also passed in
five different caustic textures per cycle, which looped around in a switch statement which
was dependent on time. The overall caustic color, then, was computed as a weighted sum of
```

the five caustic texture samples.

### Ship Rendering

The ship is an obj file with several shapes, each with several texture mappings, which are
rendered in vertex and fragment shaders much like the helmets in assignment 2. An ex-
ception is that the ship sails come with a transparency texture, while the ship body does
not. Therefore, we enable GLBLEND, use one fragment shader to render the body of the
ship, and use another fragment shader that incorporates transparency to render the ship sails.

As a result of ship sail transparency and depth shading, parts of the translucent sail obfus-
cated non-transparent background objects during rendering. To combat this we implemented
fragment discarding in the ship sail fragment shader, ignoring any fragments with an alpha
value less than .9. This eliminates the undesired translucence effect.

```
Pre- Fragment Discarding Post- Fragment Discarding
```
### Fog

We added an exponential fog effect by we altering the vertex shader to output a visibility
value related to the distance from the camera. In the fragment shader, we ’mix’ the fragment
color with a background water color by this visibility. This has the effect of obscuring far-off
objects while leaving close-objects appear unaltered. However, since we felt the scene needed
more bright textures, we added in jellyfish, which imitated bioluminescence via a bright a
default color. To make the jellyfish appear ‘lit,’ we created a separate fragment shader,
which by artificially inflating the visibility value, reduced the fog effect. The end result is
that the jellyfish is visible through the fog for a longer distance, and appears to be a light
source.


## Challenges

One of the largest challenges was the implementation of the caustic temporal function. We
followed the approach outlined by the tutorial, but the source code was not compatible with
the rendering pipeline established in the starter code. For this reason, we had to generate
the images separately using the other code base, write them to file, and then apply them as
caustic textures.

Another challenge was getting the caustic textures to apply across all objects in the scene.
Some of our objects had normal mapping; some used Phong shading and others Lambertian;
all had different fragment shaders. We made this process as easy as possible by supporting
the caustic uv, passing this value through the shared vertex shader to all relevant fragment
shader textures. Then each fragment shader had to implement the multiplicative behavior
of the caustic texture blending individually based on its lighting schema.

The ship was a challenge to render because it is an obj file made of several shapes. Before
trying to render the ship, we had no idea an .obj file could have multiple shapes, and so
it was only after intense debugging that we realized we needed to modify the ReadMesh
function (in main.cpp) by adding the vertices and triangles of each shape returned from the
call to ReadFromObjFile into a cumulative triangle mesh.

## Results

Some nice result images are shown in the following pages. See our attached video in the
upload additionally for a full rendering and animation effect.



## Contributions

Anna worked on the Perlin noise implementation of terrain generation, as well as the caustics
effect and the caustics animation. Ben worked on the fog and asset modelling and placement
(ship, rocks, and jellyfish).

References:

1. Ship: https://www.cgtrader.com/free-3d-models/watercraft/other/ghost-ship
2. Kelp: https://sketchfab.com/3d-models/kelp-83202894d3f64a129d7fdeefc044aed
3. Rock Cliff: https://www.cgtrader.com/free-3d-models/exterior/landscape/rocky-cliff
4. Rocks https://www.cgtrader.com/free-3d-models/plant/other/lowpoly-pbr-rocks-and-
    foliage
5. Jellyfish: https://sketchfab.com/3d-models/jellyfish-8787dadc26454cc9b78c49fc32012dbf
6. Caustics: https://www.opengl.org/archives/resources/code/samples/mjktips/caustics/
7. Perlin Noise tutorial: https://solarianprogrammer.com/2012/07/18/perlin-noise-cpp-
    11/
