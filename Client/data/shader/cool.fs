#version 120

uniform sampler2D tex;
uniform vec2 uStep;

const int gaussRadius = 11;
const float gaussFilter[gaussRadius] = float[gaussRadius](
	0.0402,0.0623,0.0877,0.1120,0.1297,0.1362,0.1297,0.1120,0.0877,0.0623,0.0402
);

void main()
{
	vec2 texCoord = gl_TexCoord[0].st - float(int(gaussRadius / 2)) * uStep;
	vec3 sum = vec3(0.0);

	for(int i = 0; i < gaussRadius; i++)
	{
		sum += gaussFilter[i] * texture2D(tex, texCoord).xyz;
		texCoord += uStep;
	}

	sum = min(sum,vec3(1.0));
	
	gl_FragColor = vec4(sum,1.0);
}