#version 430
#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D uGPosition;
uniform sampler2D uGNormal;
uniform sampler2D uGAmbient;
uniform sampler2D uGDiffuse;
uniform sampler2D uGlossyShininess;
uniform sampler2D uGShadingDepth;

uniform samplerCube uSkyboxSampler;

uniform vec3 uDirectionalLightDir;
uniform vec3 uDirectionalLightIntensity;

uniform vec3 uPointLightPosition;
uniform vec3 uPointLightIntensity;
uniform vec3 uAmbiantLightIntensity;

uniform float uShadowMapBias;

uniform bool uCastShadow;

out vec3 fColor;

uniform sampler2DShadow uShadowLightMap[10];

vec2 poissonDisk[16] = vec2[](
	vec2( -0.94201624, -0.39906216 ),
	vec2( 0.94558609, -0.76890725 ),
	vec2( -0.094184101, -0.92938870 ),
	vec2( 0.34495938, 0.29387760 ),
	vec2( -0.91588581, 0.45771432 ),
	vec2( -0.81544232, -0.87912464 ),
	vec2( -0.38277543, 0.27676845 ),
	vec2( 0.97484398, 0.75648379 ),
	vec2( 0.44323325, -0.97511554 ),
	vec2( 0.53742981, -0.47373420 ),
	vec2( -0.26496911, -0.41893023 ),
	vec2( 0.79197514, 0.19090188 ),
	vec2( -0.24188840, 0.99706507 ),
	vec2( -0.81409955, 0.91437590 ),
	vec2( 0.19984126, 0.78641367 ),
	vec2( 0.14383161, -0.14100790 )
);

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

struct DirectionnalLightShadow
{
    mat4 lightViewProjMatrix;
    int shadowMapLayer;
    float shadowMapBias;
};

uniform sampler2DShadow uDirLightShadowMap;


layout (std430, binding=3) buffer bShadowData
{
	mat4 shadowLightViewProjMatrix[];
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
	float depth = texelFetch(uGShadingDepth, ivec2(gl_FragCoord.xy), 0).r;
	vec3 position = vec3(texelFetch(uGPosition, ivec2(gl_FragCoord.xy), 0));
	if (depth >= 1.0)
	{
		fColor = texture(uSkyboxSampler, position).xyz;
	}
	else
	{
		vec3 normal = normalize(vec3(texelFetch(uGNormal, ivec2(gl_FragCoord.xy), 0)));
		vec3 diffuse = vec3(texelFetch(uGDiffuse, ivec2(gl_FragCoord.xy), 0));
		vec3 ambient = vec3(texelFetch(uGAmbient, ivec2(gl_FragCoord.xy), 0));
		vec4 shininess = texelFetch(uGlossyShininess, ivec2(gl_FragCoord.xy), 0);

		vec3 color = directionalColor(uDirectionalLightIntensity, uDirectionalLightDir, normal, diffuse, shininess) +
					 pointColor(uPointLightIntensity, uPointLightPosition, position, normal, diffuse, shininess) +
					 ambiantColor(uAmbiantLightIntensity, ambient);
		for (int i=0; i<dirLights.length();++i)
		{
			if (uCastShadow)
			{
				vec4 positionInDirLightScreen = shadowLightViewProjMatrix[i] * vec4(position, 1);
				vec3 positionInDirLightNDC = vec3(positionInDirLightScreen / positionInDirLightScreen.w) * 0.5 + 0.5;
				float dirLightVisibility = textureProj(uShadowLightMap[i],
														vec4(positionInDirLightNDC.xy,
															positionInDirLightNDC.z - uShadowMapBias, 1.0),
														0);
				if (dirLightVisibility > 0.0)
				{
					for (int i = 0; i < 3; ++i)
					{
						// Noisy shadows:
						int index = int(dot(gl_FragCoord.xy, position.xy)*(i+1)+
										int(gl_FragCoord.x) % (i+1)+
										int(gl_FragCoord.y) % (i+1)+
										position.z * i) % 16;

						dirLightVisibility += textureProj(uShadowLightMap[i],
														  vec4(positionInDirLightNDC.xy + 0.0002 * poissonDisk[index],
															   positionInDirLightNDC.z - uShadowMapBias, 1.0),
														  0.0);
					}
					dirLightVisibility *= 0.25;
					color += directionalColor(dirLights[i].color.rgb, dirLights[i].direction.xyz, normal, diffuse, shininess)
							* dirLightVisibility;
				}
				//color = dirLightVisibility * vec3(1,1,0);
			}
			else
			{
				color += directionalColor(dirLights[i].color.rgb, dirLights[i].direction.xyz, normal, diffuse, shininess);
			}
			//color = vec3(100, 0,0) * shadowLightViewProjMatrix[i][int(4*gl_FragCoord.x / 1200)][int(4*(1-gl_FragCoord.y / 900))];

			//if (gl_FragCoord.x < 700 && gl_FragCoord.y < 700)
				//color += vec3(texelFetch(uShadowLightMap[i], ivec2(gl_FragCoord.xy), 0));
		}

		for (int i=0; i<pointLights.length();++i)
			color += pointColor(pointLights[i].color.rgb, pointLights[i].position.xyz, position, normal, diffuse, shininess);

		fColor = color;
	}
}
