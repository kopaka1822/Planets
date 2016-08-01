#version 120

varying vec2 curpos;
varying vec3 col;
varying vec2 rtexel;

uniform vec2 texmove;
uniform float brightFactor; // default 0.2

void main()
{
	vec3 luma = vec3( 0.299 , 0.587 , 0.114);
	float luminance = dot(luma, gl_Color.rgb);
	float darkness = 1 - luminance * 0.5;
	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(gl_Vertex.xy,0.0,1.0);
	curpos = gl_Vertex.xy; // between 0 and 1
	col = gl_Color.rgb * darkness * brightFactor; // darken the color
	rtexel = texmove; // 0 - 1
}