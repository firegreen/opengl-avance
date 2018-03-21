#version 430
#ifdef GL_ES
precision mediump float;
#endif

in vec3 vTexCoords;
uniform samplerCube uSkyboxSampler;

out vec3 fColor;

void main()
{
	//fColor = vec3(1,1,0.2);
	fColor = texture(uSkyboxSampler, vTexCoords).xyz;
}
