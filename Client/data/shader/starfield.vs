#version 330
#pragma optimize(off)

uniform vec2 rtexel;
uniform uint offset;

uint intmod(uint a, uint b)
{
	return a - (a / b) * b;
}

uint randomInt(uint n)
{
	uint val = uint(n) * uint(541) + uint(463);
	return intmod(val,1000u);
}

float toFloat(uint n)
{
	return (float(n) / 500.0) - 1.0;
}

float random(uint n)
{
	return sin(float(n)) * 1.1;
}

out float intense;

void main()
{
	uint r = randomInt(uint(gl_InstanceID) + offset);
	uint r2 = randomInt(r);
	uint r3 = randomInt(r2);
	
	vec2 pos = vec2(random(r),random(r2));
	gl_Position = vec4(pos,0.0,1.0);
	
	// intensity
	float val = ((random(r3) + 1.0) / 2.0) * 0.3 + 0.2;
	
	float phi = 0.0;
	
	float modfac = float(intmod(uint(gl_InstanceID), 80u )) + 1.0;
	phi = (rtexel.y * 6.28 * modfac + float(gl_InstanceID));
	
	
	intense = max(sin(phi),val);
}