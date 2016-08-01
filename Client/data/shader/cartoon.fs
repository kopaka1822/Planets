#version 120

uniform sampler2D bg; // background

const float bb = 0.45;

void main()
{
	vec3 c00 = texture2D(bg, gl_TexCoord[5].xy).rgb; 
	vec3 c10 = texture2D(bg, gl_TexCoord[1].xy).rgb; 
	vec3 c20 = texture2D(bg, gl_TexCoord[2].zw).rgb; 
	vec3 c01 = texture2D(bg, gl_TexCoord[3].xy).rgb; 
	vec3 c11 = texture2D(bg, gl_TexCoord[0].xy).rgb; 
	vec3 c21 = texture2D(bg, gl_TexCoord[4].xy).rgb; 
	vec3 c02 = texture2D(bg, gl_TexCoord[1].zw).rgb; 
	vec3 c12 = texture2D(bg, gl_TexCoord[2].xy).rgb; 
	vec3 c22 = texture2D(bg, gl_TexCoord[6].xy).rgb;
	
	vec3 dt = vec3(1.0,1.0,1.0); 

	float d1=dot(abs(c00-c22),dt);
	float d2=dot(abs(c20-c02),dt);
	float hl=dot(abs(c01-c21),dt);
	float vl=dot(abs(c10-c12),dt);
	float d = bb*(d1+d2+hl+vl)/(dot(c11,dt)+0.15);

	float lc = 5.0*length(c11); 
	lc = 0.2*(floor(lc) + pow(fract(lc),4.0));
	c11 = 4.0*normalize(c11); 
	vec3 frct = fract(c11); frct*=frct;
	c11 = floor(c11) + frct*frct;
	c11 = 0.25*(c11)*lc; lc*=0.577;
	c11 = mix(c11,lc*dt,lc);
	gl_FragColor = vec4((1.1-pow(d,1.5))*c11,1.0);
}