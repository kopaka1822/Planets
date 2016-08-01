#version 120

uniform sampler2D tex;
uniform float factor;


void main()
{
	vec3 color = factor * texture2D(tex, gl_TexCoord[0].st).rgb;
	
	gl_FragColor = vec4(color,1.0);
}