#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

uniform sampler2D u_Textures[3];
uniform float u_TexIndex;
uniform float u_Scale;

void main()
{
	int index = int(u_TexIndex);
	vec4 texColor = texture(u_Textures[index], v_TexCoord);
	o_Color = texColor;
};