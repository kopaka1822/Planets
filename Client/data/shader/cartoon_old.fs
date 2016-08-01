#version 120

uniform sampler2D bg; // background

void main()
{
	vec3 uy = texture2D(bg, gl_TexCoord[1].xy).xyz; 
	vec3 lx = texture2D(bg, gl_TexCoord[4].xy).xyz; 
	vec3 ct = texture2D(bg, gl_TexCoord[0].xy).xyz; 
	vec3 rx = texture2D(bg, gl_TexCoord[5].xy).xyz; 
	vec3 dy = texture2D(bg, gl_TexCoord[2].xy).xyz; 
	vec3 dt = vec3(1.0,1.0,1.0);
	
	float lc = length(ct); 
	lc = 0.2*floor(5.0*lc) + 0.1;
	ct = normalize(ct)*lc; // center 
 
	float d = (dot(abs(rx-lx),dt)+dot(abs(uy-dy),dt))/(dot(ct,dt)+0.15);

	gl_FragColor = vec4((1.1-pow(d,1.5))*ct, 1.0);
}