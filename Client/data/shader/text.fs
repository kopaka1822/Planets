varying vec2 texpos;
uniform sampler2D tex;
uniform vec4 color;

void main(void) 
{
	vec4 col = vec4(1, 1, 1, texture2D(tex, texpos).a) * color;
	gl_FragColor = col;
}