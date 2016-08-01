#version 120

uniform sampler2D bg; // background
varying vec2 posPos;
varying vec2 s; // step

uniform float FXAA_SPAN_MAX; //= 8.0;
uniform float FXAA_REDUCE_MUL; //= 0.0;//1.0 / 8.0;
const float FXAA_REDUCE_MIN = 1.0 / 128.0;
uniform float FXAA_SUBPIX_SHIFT; //= 0.5;//1.0 / 4.0; Recommended [0.1 , 1.0]

vec3 FxaaPixelShader()
{
	/*
		NW		NE
			M
		SW		SE
	*/
	
	vec3 rgbM = texture2D(bg , posPos.st).rgb;
	vec3 rgbNW = texture2D(bg, posPos.st - (s * FXAA_SUBPIX_SHIFT)).rgb;
	vec3 rgbNE = texture2D(bg, posPos.st + (vec2(s.x, -s.y) * FXAA_SUBPIX_SHIFT)).rgb;
	vec3 rgbSW = texture2D(bg, posPos.st + (vec2(-s.x, s.y) * FXAA_SUBPIX_SHIFT)).rgb;
	vec3 rgbSE = texture2D(bg, posPos.st + (s * FXAA_SUBPIX_SHIFT)).rgb;
	
	// edge detection
	vec3 luma = vec3( 0.299 , 0.587 , 0.114);
	float lumaM = dot(rgbM, luma);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	
	float lumaMin = min(lumaM, min(lumaNW, min( lumaNE, min( lumaSW, lumaSW))));
	float lumaMax = max(lumaM, max(lumaNW, max( lumaNE, max( lumaSW, lumaSW))));
	
	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));
	
	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
	
	// adjust dir magnitude
	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX), dir * rcpDirMin)) * s;
	
	vec3 rgbA = (1.0 / 2.0) * (texture2D(bg, posPos.st + dir * (1.0 / 3.0 - 0.5)).rgb + texture2D(bg, posPos.st + dir * (2.0 / 3.0 - 0.5)).rgb);
	vec3 rgbB = rgbA * (1.0 / 2.0) + (1.0 / 4.0) * (texture2D(bg, posPos.st + dir * (0.0 / 3.0 - 0.5)).rgb + texture2D(bg, posPos.st + dir * (3.0 / 3.0 - 0.5)).rgb);
	
	float lumaB = dot(rgbB, luma);
	if((lumaB < lumaMin) || (lumaB > lumaMax))
		return rgbA; // rgbB was outside of luma range
	else
		return rgbB;
}

void main()
{
	vec4 color = vec4(1.0);
	color.rgb = FxaaPixelShader();
	
	gl_FragColor = color;
}