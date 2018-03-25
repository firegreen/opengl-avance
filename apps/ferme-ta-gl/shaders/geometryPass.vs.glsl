#version 330

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 uModelViewProjMatrix;
uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

out vec3 vViewSpacePosition;
out vec3 vViewSpaceNormal;
out vec3 vTexCoords;

void main()
{
    vViewSpacePosition = (uModelViewMatrix * vec4(aPosition,1)).xyz;
    vViewSpaceNormal = normalize((uNormalMatrix * vec4(aNormal,0)).xyz);
	gl_Position = (uModelViewProjMatrix * vec4(aPosition,1));
    vTexCoords = vec3(aTexCoords,1);
}
