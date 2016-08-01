#version 120

void main()
{
	gl_Position = vec4(gl_Vertex.xy, 0.0, 1.0);
	gl_TexCoord[0] = vec4((gl_Vertex.xy + vec2(1.0)) * 0.5,0.0,0.0);
}