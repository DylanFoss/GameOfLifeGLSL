#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

//uniform sampler2D u_Texture;

void main()
{
	//vec4 texColor = texture(u_Texture, v_TexCoord);
	//o_Color = texColor * vec4(v_TexCoord, 0.0f, 1.0f);
	o_Color = vec4(v_TexCoord, 0.0f, 1.0f);
};