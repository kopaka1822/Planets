#version 120

uniform vec2 step; // dx and dy to next pixel

void main()
{
	gl_Position = vec4(gl_Vertex.xy, 0.0, 1.0);
	
	float x = step.x;
	float y = step.y;
	vec2 dg1 = vec2( x,y);
	vec2 dg2 = vec2(-x,y);
	vec2 dx  = vec2(x,0.0);
	vec2 dy  = vec2(0.0,y);
	
	vec2 texel = (gl_Vertex.xy + vec2(1.0)) * 0.5;
	gl_TexCoord[0] = vec4(texel,0.0,0.0); // bg texel
	gl_TexCoord[6] = vec4(texel,0.0,0.0); // paper texel
	
	gl_TexCoord[1].xy = gl_TexCoord[0].xy - dy;
	gl_TexCoord[2].xy = gl_TexCoord[0].xy + dy;
	gl_TexCoord[3].xy = gl_TexCoord[0].xy - dx;
	gl_TexCoord[4].xy = gl_TexCoord[0].xy + dx;
	gl_TexCoord[5].xy = gl_TexCoord[0].xy - dg1;
	gl_TexCoord[6].xy = gl_TexCoord[0].xy + dg1;
	gl_TexCoord[1].zw = gl_TexCoord[0].xy - dg2;
	gl_TexCoord[2].zw = gl_TexCoord[0].xy + dg2;
}