#version 120

varying vec2 curpos;
varying vec3 col;
varying vec2 rtexel;

uniform vec2 texmove;

void main()
{
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(gl_Vertex.xy,0.0,1.0);
	curpos = gl_Vertex.xy; // between 0 and 1
	col = gl_Color.rgb;
	rtexel = texmove; // 0 - 1
}