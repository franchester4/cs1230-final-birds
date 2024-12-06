# cs1230-final-birds
2024 cs1230 Final Project

------
copy pasting my design decisions from projects 5/6 for now

## project 5
Very similar to Intersect/Illuminate: I pulled all the shape related data into `ShapeMeta` so that things could be precomputed (e.g. inverse CTM). I did the same thing for Camera and its projection/view matrices. The general structure of `Realtime` was to read the scene into `rendered` and then extract all the shapes and lights (and camera data). I decided to bind a VBO/VAO structure to each shape, so that it makes more sense to pass in the material properties (and should set up textures nicely for the next project). Handling settings was very similar to project 1, I have update parameter functions for each of the relevant settings. When the scenefile was changed, I would reset everything.

The different shapes were handled in separate classes (in `utils/shapes`). The `ShapeMeta` class would look at the type of primitive and then extract the corresponding tessellation, according to `m_param1` and `m_param2`. Everything else was pretty much the same as Lab 10, aside from passing in the light data through uniforms (this is possible since we have at most 8 lights in a scene).

## project 6
I implemented *invert* for the per-pixel filter and *sharpen* for the kernel based filter. My default FBO is on line 47 of `realtime.cpp`, set for 2. For lights, I simply edited the shader program I had from project 5 to account for attenuation and spot light. This required making a few more uniforms. Similar to project 4, I pulled out the logic for computing `l_i`, the attenuation factor, and the spotlight factor for the light before computing the intensity.

For movement, it is all about changing the camera's position, look, and up vectors. For the keyboard commands, I iterated over the possible keys and directly changed the position: then, I updated the associated view and projection matrices using functions I had written for project 5. Similarly, in mouse commands, I pulled out the Rodrigues' matrix formula as a separate function for each kind of rotation. Then, I updated the view and projection matrices. The speed for this part is hardcoded in for what I thought was reasonable (and is easy to change).

Finally, for FBO, I just integrated Lab 11 into the project. I created a filter shader program to handle all the filtering, and these had uniforms for whether or not per-pixel and/or kernel-based filtering was set in the current settings. The filters are then implemented directly in the fragment shader. In `realtime.cpp`, everything is the same as Lab 11 except for the few uniforms I added in (namely the ones I mentioned along with screen width + height).
The only relevant refactoring was pulling out the paintGL logic from project 5 into `paintScene`, similar to `paintExampleGeometry` in Lab 11.
