#version 330

uniform vec2 rtexel;
uniform float scalar;
uniform uint offset;

uint randomInt(uint n)
{
	uint val = uint(n) * uint(541) + uint(463);
	return uint(mod(val,1000.0));
}

float toFloat(uint n)
{
	return (float(n) / 500.0) - 1.0;
}

uint fac(uint n)
{
	uint v = (n * n + n) / uint(2);
	
	return v;
}

float random(uint n)
{
	return sin(float(fac(uint(n + uint(1))))) * 1.1;
	//return sin(float(n)) * 1.1;
}

out float intense;

void main()
{
	uint r = randomInt(uint(gl_InstanceID) + offset);
	uint r2 = randomInt(r);
	uint r3 = randomInt(r2);
	
	gl_Position = vec4(random(r),random(r2),0.0,1.0);
	gl_PointSize = scalar * 18.0; // in screen coords
	
	// intensity
	
	float val = ((random(r3) * 2.0) - 1.0) * 0.3 + 0.7;
	
	float phi = 0.0;
	
	float modfac = mod(gl_InstanceID, 80) + 1.0;
	phi = (rtexel.y * 6.28 * modfac + float(gl_InstanceID));
	
	
	intense = max(sin(phi),val);
}