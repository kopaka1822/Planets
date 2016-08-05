#version 330 core

uniform sampler2D tex;
uniform vec3 color;

in float intense;

void main()
{
	gl_FragColor = vec4(texture(tex, gl_PointCoord).rgb * color * intense, 1.0);
}