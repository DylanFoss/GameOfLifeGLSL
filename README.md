# GameOfLifeGLSL

An implementation of Conway's Game of Life utilising shaders with GLSL and openGL, allowing for large simulations to be run quickly on the GPU.

![GlidersInMotion](https://user-images.githubusercontent.com/57671477/215569681-6c0ccc55-50cc-44b8-a5d7-6ea022bd7c64.gif)

### Demonstration

[Basic Demonstration](https://www.youtube.com/watch?v=0dnzAxGN9AU)

[Gamespace Scaling Demo](https://www.youtube.com/watch?v=_ptRF3jm1ns)

[Trails Demo](https://www.youtube.com/watch?v=PycTTEz7Pbk)

[Heatmap Trail Demo](https://www.youtube.com/watch?v=R-drmOVpYfc)

## How it works

This implementation stores the state of the GoL via two textures which both match the size of the game space. One serves as the read texture with the state and the other is written to via a shader, using the rules to the GoL. Each step the read/write textures swap, and a final texture is writen to using the most recent state texture. This is then rendered to a quad on screen. The state is currently initialised using a noise generating shader, which then uses a threshold to cut the texel values to 0.0 or 1.0.

## Controls

| Action | Description |
| --- | --- |
| WASD | Pan camera |
| Middle mouse button + move mouse | Move camera around cursor |
| Scrollwheel | Zoom in/out of cursor|
| Space | Pause simulation |
| Left Click | Kill/Create a cell |
| G | Toggle gridlines |

## Command Line Arguments

| Argument Number | Description | Input |
| --- | --- |  --- |
| 1&2 | Width and height of window, capped to monitors size. | Positive integer value (default: 800) |
| 3&4 | Width and height of simulation game space | Positive integer value (default: 800) |
| 5 | Rendering mode of simul;ation, affects how the simulation looks | 'Basic', 'Trails' or 'Heatmap' (default: 'Basic') |

## Planned Features:

- Allowing realtime modifying of rulessets using a command console.
- Add speed controls to allow the speed up the simulation.
- Savestates, allowing to try sample scenarios and recovering to a known state.
- Functionality to allow the user to change visuals via parameters.
