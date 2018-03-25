#version 430
#ifdef GL_ES
precision mediump float;
#endif

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
uniform sampler2D uNormalSampler;

struct PointLight
{
    vec4 color;
    vec4 position;
};

struct DirectionnalLight
{
    vec4 color;
    vec4 direction;
};

layout (std430, binding=2) buffer bPointLightData
{
    PointLight pointLights[];
};

layout (std430, binding=1) buffer bDirLightData
{
    DirectionnalLight dirLights[];
};

vec3 blingPhong(vec3 lightDir, vec3 eyeDir, vec3 N)
{
    float dotWN = max(0.0, dot(N, -uDirectionalLightDir));
    float dotHalfVN = max(0.0, dot(N,(-uDirectionalLightDir+eyeDir)*0.5f));
    return (uKd*dotWN+uKs*dotHalfVN);
}

vec3 blingPhongShininess(vec3 lightDir, vec3 eyeDir, vec3 N)
{
    float dotWN = max(0.0, dot(N, -lightDir));
    float dotHalfVN = pow(max(0.0, dot(N,(-lightDir+eyeDir)*0.5f)),uShininess);
    return (uKd*dotWN+uKs*dotHalfVN);
}

vec3 directionalColor(vec3 lightColor, vec3 lightDir, vec3 N)
{
    return lightColor * blingPhong(lightDir, vec3(0,0,1), N);
}

vec3 pointColor(vec3 lightColor, vec3 lightPos, vec3 N)
{
    float distToPointLight = length(uPointLightPosition - vViewSpacePosition);
    vec3 dirToObject = (vViewSpacePosition-uPointLightPosition) / distToPointLight;
    return (lightColor * blingPhongShininess(dirToObject, vec3(0,0,1), N)) / (distToPointLight * distToPointLight);
}

vec3 ambiantColor(vec3 lightColor)
{
    return uAmbiantLightIntensity * uKa;
}

void main()
{
    vec3 N = normalize(vViewSpaceNormal);
    vec3 color = directionalColor(uDirectionalLightIntensity, uDirectionalLightDir, N) +
            pointColor(uPointLightIntensity, uPointLightPosition, N) +
            ambiantColor(uAmbiantLightIntensity);



    for (int i=0; i<dirLights.length();++i)
        color += directionalColor(dirLights[i].color.rgb, dirLights[i].direction.xyz, N);
    for (int i=0; i<pointLights.length();++i)
        color += pointColor(pointLights[i].color.rgb, pointLights[i].position.xyz, N);
    if (uUseTexture)
        color *= vec3(texture(uKdSampler, vTexCoords.xy));
    fColor = color;
}
