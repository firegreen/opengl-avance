#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec3 vTexCoords;

out vec3 fColor;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;
uniform vec3 uKd;

uniform sampler2D uKdSampler;

void main()
{
    vec3 color = vec3(texture(uKdSampler, vTexCoords.xy)) * uKd;
    float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
    vec3 dirToPointLight = (uPointLightPosition - vViewSpacePosition) / distToPointLight;
    fColor = color * (uDirectionalLightIntensity * max(0.0, dot(vViewSpaceNormal, -uDirectionalLightDir))
                    + uPointLightIntensity * max(0.0, dot(vViewSpaceNormal, dirToPointLight)) / (distToPointLight * distToPointLight));
}
