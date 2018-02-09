#version 430

in vec3 vViewSpacePosition;
in vec3 vViewSpaceNormal;
in vec3 vTexCoords;

layout(location = 0) out vec3 fPosition;
layout(location = 1) out vec3 fNormal;
layout(location = 2) out vec3 fAmbient;
layout(location = 3) out vec3 fDiffuse;
layout(location = 4) out vec4 fGlossyShininess;

uniform vec3 uKd;
uniform vec3 uKs;
uniform vec3 uKa;
uniform float uShininess;

uniform bool uUseDTexture;
uniform bool uUseATexture;
uniform bool uUseSpecTexture;
uniform bool uUseShinTexture;

uniform sampler2D uKdSampler;
uniform sampler2D uKaSampler;
uniform sampler2D uKspecSampler;
uniform sampler2D uKshinSampler;
uniform sampler2D uNormalSampler;

void main()
{
    fPosition = vViewSpacePosition;
    fNormal = vViewSpaceNormal;

    fAmbient = uKa;
    if (uUseATexture)
        fAmbient *= vec3(texture(uKaSampler, vTexCoords.xy));

    fDiffuse = uKd;
    if (uUseDTexture)
        fDiffuse *= vec3(texture(uKdSampler, vTexCoords.xy));

    vec3 specular = uKs;
    if (uUseSpecTexture)
        specular *= vec3(texture(uKspecSampler, vTexCoords.xy));

    float shin = uShininess;
    if (uUseShinTexture)
        shin *= vec3(texture(uKshinSampler, vTexCoords.xy));

    fGlossyShininess = vec4(specular,shin);
}
