#version 120

uniform vec2 step; // dx and dy to next pixel

varying vec4 posPos;
varying vec2 s; // step

void main()
{
	// position [-1 , 1]
	gl_Position = vec4(gl_Vertex.xy, 0.0, 1.0);
	s = step;
	
	// setting texel [0, 1]
	vec2 texel = (gl_Vertex.xy + vec2(1.0)) * 0.5;
	gl_TexCoord[0] = vec4(texel,0.0,0.0); // bg texel
	
	posPos.xy = texel.xy;
	//posPos.zw = texel.xy - (step * (0.5 + FXAA_SUBPIX_SHIFT));
}