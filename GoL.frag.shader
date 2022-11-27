#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

uniform sampler2D u_State;
uniform vec2 u_Scale;

int Get(int x, int y)
{
	vec4 texColor = texture(u_State, (v_TexCoord+(vec2(x,y)/u_Scale)));
	return texColor.r > 0.5 ? 1 : 0;
}

void main() {

    int sum = Get(-1, -1) +
        Get(-1, 0) +
        Get(-1, 1) +
        Get(0, -1) +
        Get(0, 1) +
        Get(1, -1) +
        Get(1, 0) +
        Get(1, 1);

    if (sum == 3)
    {
       o_Color = vec4(vec3(1.0), 1.0);
    }
    else if (sum == 2)
    {
        float current = float(Get(0, 0));
        o_Color = vec4(vec3(current), 1.0);
    }
    else
    {
        o_Color = vec4(vec3(0.0), 1.0);
    }

    //vec4 texColor = texture(u_State, v_TexCoord + vec2(1, 0)/80);
    //o_Color = texColor;
}