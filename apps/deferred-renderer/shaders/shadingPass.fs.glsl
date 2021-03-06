#version 430
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uGPosition;
uniform sampler2D uGNormal;
uniform sampler2D uGAmbient;
uniform sampler2D uGDiffuse;
uniform sampler2D uGlossyShininess;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;
uniform vec3 uAmbiantLightIntensity;

out vec3 fColor;

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

vec3 blingPhong(vec3 lightDir, vec3 eyeDir, vec3 N, vec3 diffuse, vec3 specular)
{
    float dotWN = max(0.0, dot(N, -uDirectionalLightDir));
    float dotHalfVN = max(0.0, dot(N,(-uDirectionalLightDir+eyeDir)*0.5f));
    return (diffuse*dotWN+specular*dotHalfVN);
}

vec3 blingPhongShininess(vec3 lightDir, vec3 eyeDir, vec3 N, vec3 diffuse, vec3 specular, float shininess)
{
    float dotWN = max(0.0, dot(N, -lightDir));
    float dotHalfVN = pow(max(0.0, dot(N,(-lightDir+eyeDir)*0.5f)),shininess);
    return (diffuse*dotWN+specular*dotHalfVN);
}

vec3 directionalColor(vec3 lightColor, vec3 lightDir, vec3 N, vec3 diffuse, vec4 shininess)
{
    return lightColor * blingPhong(lightDir, vec3(0,0,1), N, diffuse, shininess.rgb);
}

vec3 pointColor(vec3 lightColor, vec3 lightPos, vec3 N, vec3 position, vec3 diffuse, vec4 shininess)
{
    float distToPointLight = length(lightPos - position);
    vec3 dirToObject = (position-position) / distToPointLight;
    return (lightColor * blingPhongShininess(dirToObject, vec3(0,0,1), N, diffuse, shininess.rgb, shininess.a)) /
            (distToPointLight * distToPointLight);
}

vec3 ambiantColor(vec3 lightColor, vec3 uKa)
{
    return uAmbiantLightIntensity * uKa;
}

void main()
{
    vec3 position = vec3(texelFetch(uGPosition, ivec2(gl_FragCoord.xy), 0)); // Correspond a vViewSpacePosition dans le forward rendere
    vec3 normal = normalize(vec3(texelFetch(uGNormal, ivec2(gl_FragCoord.xy), 0)));
    vec3 diffuse = vec3(texelFetch(uGDiffuse, ivec2(gl_FragCoord.xy), 0));
    vec3 ambient = vec3(texelFetch(uGAmbient, ivec2(gl_FragCoord.xy), 0));
    vec4 shininess = texelFetch(uGlossyShininess, ivec2(gl_FragCoord.xy), 0);

    vec3 color = directionalColor(uDirectionalLightIntensity, uDirectionalLightDir, normal, diffuse, shininess) +
                 pointColor(uPointLightIntensity, uPointLightPosition, position, normal, diffuse, shininess) +
                 ambiantColor(uAmbiantLightIntensity, ambient);

    for (int i=0; i<dirLights.length();++i)
        color += directionalColor(dirLights[i].color.rgb, dirLights[i].direction.xyz, normal, diffuse, shininess);
    for (int i=0; i<pointLights.length();++i)
        color += pointColor(pointLights[i].color.rgb, pointLights[i].position.xyz, position, normal, diffuse, shininess);

    fColor = color;
}
