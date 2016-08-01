#version 120

varying vec2 curpos;
varying vec3 col;
varying vec2 rtexel;

uniform sampler2D noise;
uniform vec4 subcol;

void main()
{
	// Parameters:
	// threshold for pulsalting 'lava see'
	const float threshold = 0.7; // threshold for lava see
	const float invthresh = 1.0 / threshold;
	
	// value noise samples
	vec2 off0 = vec2(rtexel.y,0.5); // 0.1 0.5
	vec2 off1 = vec2(0.1,0.804);
	vec2 off2 = vec2(0.7,0.454);
	
	
	vec4 outcol = vec4(0.0); // transparent
	float dist = distance(vec2(0.5),curpos);
	
	if( dist <= 0.5 )
	{
		// take sample pixels from "ball"
		vec2 samplePos = sqrt(dist) * (curpos - vec2(0.5)) * 1.0;
		float valueNoise = 0.0;
		
		// generate random pattern
		valueNoise += texture2D(noise, samplePos / 8.0 + off0).r;
		valueNoise += texture2D(noise, samplePos / 4.0 + off1).r * 0.5;
		valueNoise += texture2D(noise, samplePos / 2.0 + off2).r * 0.25;
		
		valueNoise /= 1.8;
		
		
		vec3 dstCol = col.rgb * valueNoise;
		if(valueNoise < threshold)
		{
			// 'lava see'
			float mixPercent = (threshold - valueNoise) * invthresh; // [0 , 1] 0 border
			
			float twopi = 6.28318;
			// rtexel.x [0 , 1]
			valueNoise *= 1.0 - (1.0 + sin(twopi * rtexel.x)) * 0.5 * (threshold - valueNoise) * invthresh;
			valueNoise = max(valueNoise, 0.3);
			vec3 lavaCol = subcol.rgb * valueNoise;
			
			vec3 mixcol = mix(dstCol,lavaCol.rgb,mixPercent);
			outcol = vec4(mixcol.rgb, 1.0);
		}
		else
		{
			outcol = vec4(dstCol, 1.0);
		}
		
		// smooth alpha on edges
		float fac = dist * 2.0; // 0 - 1
		float alpha = 1.0 - pow(fac, 4.0);
		
		outcol.a = alpha;
	}
	
	gl_FragColor = outcol;
}