#shader fragment
#version 330 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

uniform vec2 u_Scale;
uniform vec2 u_GridDimensions;
uniform float u_Zoom;

float grid(vec2 st, float res)
{
	vec2 grid = fract(st * res);
	return (step(res, grid.x) * step(res, grid.y));
}

void main()
{
	vec2 digit_uv = v_TexCoord.xy * u_Scale.x; // scale
	float x = grid(digit_uv, u_Scale.y); // resolution
	o_Color.rgb = vec3(0.2);
	o_Color.a = 1 - x;

	//float blur = .5; // between 0 and .5, smaller value is more blurred
	//if (u_GridDimensions.x * u_Zoom > 50.)
	//{
	//	mix(.5, .1, 1 - exp(-5 * ((u_GridDimensions.x * u_Zoom) * 0.0002)));
	//}

	//float blur = mix(1, 0, 1 - exp(-5 * ((u_GridDimensions.x * u_Zoom) * 0.0006)));
	//float blur;
	//if (blur < 0.1) blur = 0;
	////else if (blur > 0.6) blur = 1;

	//if (u_GridDimensions.x * u_Zoom < 50)
	//{
	//	blur = 1;
	//}
	//else
	//{
	//	blur = mix(1, 0, ((u_GridDimensions.x * u_Zoom) * 0.006));
	//	if (blur < 0.1) blur = 0;
	//}

	//float a = clamp((u_Scale.x - x - max(u_Scale.x, u_Scale.y) + 1.) * blur, .0, 1.);
	//o_Color.rgba = mix(vec4(0.), vec4(vec3(0.2),1.0), a);
	////o_Color.rgb = mix(vec3(0.), vec3(0.2), a);
}