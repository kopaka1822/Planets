#version 120

uniform sampler2D tex;

varying vec2 texel;

const vec3 lumConst = vec3( 0.2126, 0.7152, 0.0722 );


void main()
{
	vec3 color = texture2D(tex, gl_TexCoord[0].st).rgb;
	
	if(dot(color,lumConst) < 0.90)
	{
		color = vec3(0.0);
	}
	else
	{
		// add brightness
		color = (color + vec3(1.0f)) * 0.5;
	}
	
	gl_FragColor = vec4(color,1.0);
}