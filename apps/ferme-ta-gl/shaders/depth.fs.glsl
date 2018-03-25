#version 430
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uGDepth;

out vec3 fColor;

void main()
{
	float depth = texelFetch(uGDepth, ivec2(gl_FragCoord.xy), 0).r;
	fColor = vec3(depth*depth); // Since the depth is between 0 and 1, pow it to darkness its value
}
