# GameOfLifeGLSL

An implementation of Conway's Game of Life utilising shaders with GLSL and openGL, allowing for large simulations to be run quickly on the GPU.

## An 800 * 800 simulation

![image](https://user-images.githubusercontent.com/57671477/204124335-2af2c551-325e-4eb8-9123-fda21a277b71.png)

### Demonstration

[Basic Demonstration](https://www.youtube.com/watch?v=0dnzAxGN9AU)

[Gamespace Scaling Demo](https://www.youtube.com/watch?v=_ptRF3jm1ns)

## How it works

This implementation stores the state of the GoL via two textures which both match the size of the game space. One serves as the read texture with the state and the other is written to via a shader, using the rules to the GoL. Each step the read/write textures swap, and a final texture is writen to using the most recent state texture. This is then rendered to a quad on screen. The state is currently initialised using a noise generating shader, which then uses a threshold to cut the texel values to 0.0 or 1.0.

## Controls

WASD to pan camera.

Space to pause simulation.

Left Click to add life to or kill a cell.

Mouse wheel to zoom in and out.

## To-Do:

- Allow customisation of the game parameters via keypresses/ui.
- Add speed controls to allow the speed up the simulation.
