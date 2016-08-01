#version 120

uniform sampler2D bg; // background
uniform sampler2D txPaper; // Paper

void main()
{
	vec3 ink = vec3(0.375,0.365,0.365);
	vec3 paper = texture2D(txPaper, gl_TexCoord[6].st).rgb;
	//paper = vec3(paper.x,1.0,1.0);

	vec3 uy = texture2D(bg,gl_TexCoord[1].st).rgb;
	vec3 lx = texture2D(bg,gl_TexCoord[4].st).rgb;
	vec3 ct = texture2D(bg,gl_TexCoord[0].st).rgb;
	vec3 rx = texture2D(bg,gl_TexCoord[5].st).rgb;
	vec3 dy = texture2D(bg,gl_TexCoord[2].st).rgb;
	
	vec3 dt = vec3(1.0);
	float d = dot(abs(lx-rx),dt) / (dot(lx+rx,dt) + 0.15);
	d += dot(abs(uy-dy),dt) / (dot(uy+dy,dt) + 0.15);
	
	d *= 2.0;
	
	float t = 0.127; // threshold for drawing
	if(d < t)
	{
		gl_FragColor = vec4(paper,1.0);
	}
	else
	{
		gl_FragColor = vec4(ink * (1.0-(d*d)),1.0);
	}
}