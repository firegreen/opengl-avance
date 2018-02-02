#version 330

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec3 vTexCoords;

out vec3 fColor;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;
uniform vec3 uAmbiantLightIntensity;

uniform vec3 uKd;
uniform vec3 uKs;
uniform vec3 uKa;
uniform float uShininess;

uniform bool uUseTexture;

uniform sampler2D uKdSampler;

vec3 blingPhong()
{
    float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
    vec3 dirToPointLight = (uPointLightPosition - vViewSpacePosition) / distToPointLight;
    vec3 N = normalize(vViewSpaceNormal);
    float dotDirWN = max(0.0, dot(N, -uDirectionalLightDir));
    float dotDirHalfVN = max(0.0, dot(N,(-uDirectionalLightDir+vec3(1,0,0))*0.5f));
    float dotPointWN = max(0.0, dot(N, dirToPointLight));
    float dotPointHalfVN = pow(max(0.0, dot(N,(-uDirectionalLightDir+vec3(1,0,0))*0.5f)),uShininess);

    return uDirectionalLightIntensity * (uKd*dotDirWN+uKs*dotDirHalfVN) + // dirlight
            (uPointLightIntensity/(distToPointLight * distToPointLight)) * (uKd*dotPointWN+uKs*dotPointHalfVN) + // pointlight
            uAmbiantLightIntensity * uKa;
}

void main()
{
    vec3 color = uKd;
    if (uUseTexture)
        color *= vec3(texture(uKdSampler, vTexCoords.xy));
    fColor = color*blingPhong();
    //fColor = color;
}
