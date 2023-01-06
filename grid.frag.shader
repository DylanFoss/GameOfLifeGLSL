#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

uniform vec2 u_GridDimensions;
uniform vec2 u_WindowDimensions;
uniform float u_Zoom;

uniform vec3 u_GridColour;

vec2 gridTile(vec2 uv, vec2 dimensions) 
{
    uv *= dimensions;
    return fract(uv);
}

float box(vec2 uv, vec2 size)
{

    size = (vec2(0.5) - size * 0.5);
    float thresholdWidth = 1e-2;

    vec2 newuv = smoothstep(size, size + vec2(thresholdWidth), uv);
    newuv *= smoothstep(size, size + vec2(thresholdWidth), vec2(1.0) - uv);

    //size = vec2(0.5) - size * 0.49;
    //vec2 newuv = smoothstep(size, size + vec2(1e-1), uv);
    //newuv *= smoothstep(size, size + vec2(1e-1), vec2(1.0) - uv);

    //size = vec2(0.5) - size * 0.5;
    //vec2 newuv = step(size, uv);
    //newuv *= step(size, vec2(1.0) - uv);

    return newuv.x * newuv.y;
}

void main() 
{
    vec2 uv = v_TexCoord;
    vec3 color = u_GridColour;
    float a = 1;

    // work out the number of cells to help decide how visible (if at all) the grid should be.
    float numCells = min(u_GridDimensions.x, u_GridDimensions.y) * u_Zoom;
    float pixelsPerCell = round(max(u_WindowDimensions.x, u_WindowDimensions.y)  / numCells);
    //float pixelsPerCell = min(u_WindowDimensions.x, u_WindowDimensions.y) / numCells;
    float borderSize = clamp(pixelsPerCell * 0.1, 0.1, 0.89);
    uv = gridTile(uv, u_GridDimensions);
    float gridMask = box(uv, vec2(borderSize));
    //float gridMask = box(uv, vec2(0.5));

    // alpha is decided by if we are on a gridline
    a *= (1 - gridMask);
    a *= (mix(0, 1, pixelsPerCell * 0.1));

    color = u_GridColour * (1 - gridMask);
    o_Color = vec4(color, a);
}