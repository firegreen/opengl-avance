#version 330

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 vTexCoords;

uniform mat4 uSkyProjMatrix;
uniform mat4 uSkyViewMatrix;

void main()
{
	vTexCoords = aPosition;
	gl_Position = uSkyProjMatrix * uSkyViewMatrix * vec4(aPosition*100, 1.0);
}
