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
	//o_Color = texColor * vec4(v_TexCoord, 0.0f, 1.0f);
	o_Color = texColor;

	//ivec2 size = ivec2(u_Scale, u_Scale);
	//float total = floor(v_TexCoord.x * float(size.x)) + floor(v_TexCoord.y * float(size.y));
	//bool isEven = mod(total, 2.0) == 0.0;
	//o_Color = (isEven) ? texture(u_Textures[index], v_TexCoord) : vec4(v_TexCoord, 0.0f, 1.0f);

	//o_Color = vec4(v_TexCoord, 0.0f, 1.0f);
};