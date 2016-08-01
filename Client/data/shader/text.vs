#version 120

varying vec2 texpos;

void main(void) 
{
	gl_Position = gl_ProjectionMatrix * vec4(gl_Vertex.xy, 0, 1);
	texpos = gl_Vertex.zw;
}