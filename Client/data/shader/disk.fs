#version 120

uniform vec4 color;

varying vec2 curpos;

void main()
{
	vec4 outcol = vec4(0.0); // transparent

	float dist = distance(vec2(0.5),curpos);
	
	if( 0.5 >= dist )
	{
		outcol = color;
	}
	gl_FragColor = outcol;
}