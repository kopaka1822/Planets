#version 120

varying vec2 curpos;

void main()
{
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * vec4(gl_Vertex.xy,0.0,1.0);
	curpos = gl_Vertex.xy;
}