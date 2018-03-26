#version 430


layout(location = 0) in vec3 aPosition;
uniform mat4 uDirLightViewProjMatrixShadow;
uniform mat4 uDirLightModelMatrixShadow;

void main()
{
	gl_Position =  uDirLightViewProjMatrixShadow * uDirLightModelMatrixShadow * vec4(aPosition, 1);
}
