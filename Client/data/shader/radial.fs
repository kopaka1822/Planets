#version 120

uniform sampler2D tex; // scene
uniform vec2 radialSize; //texel size

uniform float radialBlur;
uniform float radialBright;

uniform vec2 radialOrigin;

void main()
{
	vec3 sum = vec3(0.0);
	
	vec2 texcoo = gl_TexCoord[0].st + radialSize * 0.5 - radialOrigin;
	
	for(int i = 0; i < 12; i++)
	{
		float scale = 1.0 - radialBlur * (float(i) / 11.0);
		sum += texture2D(tex, texcoo * scale + radialOrigin).rgb;
	}

	gl_FragColor = vec4(sum / 12.0 * radialBright,1.0);
}