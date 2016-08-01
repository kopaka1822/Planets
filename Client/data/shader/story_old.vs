#version 120

uniform vec2 step; // dx and dy to next pixel

void main()
{
	gl_Position = vec4(gl_Vertex.xy, 0.0, 1.0);
	
	vec2 texel = (gl_Vertex.xy + vec2(1.0)) * 0.5;
	gl_TexCoord[0] = vec4(texel,0.0,0.0); // bg texel
	
	gl_TexCoord[6] = vec4(texel,0.0,0.0); // paper texel
	gl_TexCoord[1] = gl_TexCoord[0];
	gl_TexCoord[2] = gl_TexCoord[0];
	gl_TexCoord[4] = gl_TexCoord[0];
	gl_TexCoord[5] = gl_TexCoord[0];
	
	// edge detection
	gl_TexCoord[1].y -= step.y;
	gl_TexCoord[2].y += step.y;
	gl_TexCoord[4].x -= step.x;
	gl_TexCoord[5].x += step.x;
}