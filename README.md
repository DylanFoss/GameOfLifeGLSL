# GameOfLifeGLSL

An implementation of Conway's Game of Life utilising shaders with GLSL and openGL, allowing for large simulations to be run quickly on the GPU.

![image](https://user-images.githubusercontent.com/57671477/204124335-2af2c551-325e-4eb8-9123-fda21a277b71.png)

## How it works

This implementation stores the state of the GoL via two textures which are both the height and width to the game space. One serves as the read texture with the state and the other is written to via a shader, using the rules to the GoL. Each step, the read/write textures swap, and a final texture is writen to using the most recent texture and sample it to fit the window. The state is currently initialised using a noise generating shader, which then uses a threshold to cut the texel values to 0.0 or 1.0.
