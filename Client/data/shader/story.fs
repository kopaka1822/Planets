#version 120

uniform sampler2D bg; // background
uniform sampler2D txPaper; // Paper

void main()
{
	vec3 ink = vec3(0.375,0.365,0.365);
	vec3 paper = texture2D(txPaper, gl_TexCoord[6].st).rgb;

	vec3 c00 = texture2D(bg, gl_TexCoord[5].xy).xyz; 
	vec3 c10 = texture2D(bg, gl_TexCoord[1].xy).xyz; 
	vec3 c20 = texture2D(bg, gl_TexCoord[2].zw).xyz; 
	vec3 c01 = texture2D(bg, gl_TexCoord[3].xy).xyz; 
	vec3 c11 = texture2D(bg, gl_TexCoord[0].xy).xyz; // midpoint
	vec3 c21 = texture2D(bg, gl_TexCoord[4].xy).xyz; 
	vec3 c02 = texture2D(bg, gl_TexCoord[1].zw).xyz; 
	vec3 c12 = texture2D(bg, gl_TexCoord[2].xy).xyz; 
	vec3 c22 = texture2D(bg, gl_TexCoord[6].xy).xyz; 
	vec3 dt = vec3(1.0,1.0,1.0); 
	
	float d1=dot(abs(c00-c22),dt); // dred + dgreem + dblue
	float d2=dot(abs(c20-c02),dt);
	float hl=dot(abs(c01-c21),dt);
	float vl=dot(abs(c10-c12),dt);
	float d = (d1+d2+hl+vl)/(dot(c11+c10+c02+c22,dt)+0.2);
	d =  0.5*pow(d,0.5) + d;

	vec3 ori = c11;
	c11 = (1.0-0.7*d)*c11;
	

	gl_FragColor = vec4(mix(paper, c11, pow(max(min(d,1.1)-0.1,0.0),0.5)),1.0);

}