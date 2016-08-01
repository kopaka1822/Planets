#version 120

varying vec2 curpos;
varying vec3 col;
varying vec2 rtexel;

uniform sampler2D noise;

void main()
{
	// get angle to center
	const vec2 midpoint = vec2(0.5);
	
	vec2 dirVector = curpos - midpoint;
	dirVector = normalize(dirVector);
	
	float angle = dot(dirVector, vec2(1.0,0.0));
	angle /= 16.0;
	
	float noiseVal = texture2D(noise, vec2(angle + rtexel.y,rtexel.y * 2.0) ).r;
	
	/*
	noiseVal += texture2D(noise, vec2(rtexel.y, angle + rtexel.y)).r;
	
	noiseVal /= 2.0;
	noiseVal *= noiseVal;
	*/
	
	float len = length(curpos - midpoint);
	// inside circle -> len should be 0 - 0.5
	len *= 2.0; // -> len == 0 - 1
		
	float alpha = 1.0 - len * len;
	alpha = max(0.0, alpha);
	
	vec4 outcol = vec4( col * noiseVal, alpha);
	
	const float threshold = 0.5;
	if(len < threshold)
	{
		outcol = mix(vec4(col, 1.0), outcol, len / threshold);
	}
	
	gl_FragColor = outcol;
}