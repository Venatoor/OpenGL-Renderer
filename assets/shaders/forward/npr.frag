#version 460 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
in mat3 TBN;
in vec4 fragPosLightSpace;

//============== STRUCT DEFINITIONS ======================

struct Material {
	float ambient;
	vec3 diffuse;
	vec3 specular;

	float shininess;
    float reflectivity;
    float refractionRatio;

    bool useReflection;
    bool useRefraction;

	bool useSpecularMap;
	bool useDiffuseMap;
	bool useEmissiveMap;
    bool useNormalMap;


	//========= CEL SECTION
	bool useCelShading;
	bool useTonalArtMaps;
	bool useViewSpaceTexture;
	bool useOutlines;

	int bandCount;
	float bandThresholds[8];
	vec3 bandColors[8];
	float bandIntensities[8];

	int tonalArtMapCount;

	float outlineWidth;
	vec3 outlineColor;
	float specularCutOff;

};

struct Light {
	vec3 position;
	vec3 color;
};

//=================== LIGHTS ==========================

struct DirLight {
	vec3 direction;
	vec3 color;
	float intensity;
};


struct PointLight {
	vec3 position;
	vec3 color;
	float radius;
	float intensity;
};


struct SpotLight {
	vec3 position;
    vec3 direction;
    vec3 color;
    float cutOff;      // Inner cutoff angle (cosine)
    float outerCutOff; // Outer cutoff angle (cosine)
    float intensity;
    float radius;

};

//=================== UNIFORMS ========================

#define MAX_DIR_LIGHT 1
#define MAX_POINT_LIGHT 16 
#define MAX_SPOT_LIGHT 16

#define MAX_BANDS 8 
#define MAX_TONAL_MAPS 8


uniform DirLight dirLights[MAX_DIR_LIGHT];
uniform PointLight pointLights[MAX_POINT_LIGHT];
uniform SpotLight spotLights[MAX_SPOT_LIGHT];

uniform int numDirLights;
uniform int numPointLights;
uniform int numSpotLights;

uniform	sampler2D diffuse1;
uniform	sampler2D specular1;
uniform	sampler2D emissive1;
uniform sampler2D normal1;

//cel specific textureSamples

uniform sampler2DArray tonalArtMaps;
uniform sampler2D viewSpaceTexture;


uniform sampler2D shadowMap;

uniform samplerCube skybox;

uniform vec3 viewPos;

uniform Material material;

uniform bool hasAlpha;

#include "utils.glsl"  

float CalculateDiscreteBands(float intensity) {

	if ( !material.useCelShading || material.bandCount <= 1 ) {
		return intensity;
	}

	for ( int i = 0; i < material.bandCount-1; i++ ) {
		if ( intensity < material.bandThresholds[i]) {
			return float(i) / float(material.bandCount - 1);
		}
	}
	return 1.0;

}

int FindBandIndex(float intensity) {
	
	for ( int i = 0; i < material.bandCount-1; i++) {
		if ( intensity < material.bandThresholds[i]) {
			return i;
		}
	}
	return material.bandCount-1;
}

vec2 CalculateViewSpaceUV(vec3 worldPos, vec3 viewPos, vec3 normal) {
    if (!material.useViewSpaceTexture) {
        return texCoord;
    }
    
    vec3 viewDir = normalize(viewPos - worldPos);
    vec3 worldUp = vec3(0.0, 1.0, 0.0);
    vec3 right = normalize(cross(worldUp, viewDir));
    vec3 up = normalize(cross(viewDir, right));
    
    vec3 localPos = worldPos - viewPos;
    float u = dot(localPos, right) * 0.1;
    float v = dot(localPos, up) * 0.1;
    
    return vec2(u, v) + 0.5;
}

vec3 GetBandColor(int bandIndex, float bandValue, vec2 uv) {
	vec3 baseColor;
    
    if (length(material.bandColors[bandIndex]) > 0.0) {
        baseColor = material.bandColors[bandIndex];
    } else {
        float t = float(bandIndex) / float(material.bandCount - 1);
        baseColor = material.diffuse * (1.0 - t * 0.5);
    }
    
    baseColor *= material.bandIntensities[bandIndex];
    
    if (material.useTonalArtMaps && bandIndex < material.tonalArtMapCount) {
        float tonalValue = texture(tonalArtMaps, vec3(uv, bandIndex)).r;
        baseColor *= tonalValue;
    }
    
    return baseColor;
}

vec3 CalculateDirLight(DirLight dirLight, vec3 normal, vec3 viewDir, vec3 sampledDiffuse, 
                       vec3 sampledSpecular) {

	vec3 lightDir = normalize(-dirLight.direction);

	float bandValue;
	int bandIndex;

	float ndotl = max(dot(normal,lightDir),0.0);

	if ( material.useCelShading && material.bandCount > 1 ) {

		bandValue = CalculateDiscreteBands(ndotl);
		bandIndex = FindBandIndex(ndotl);
	}
	else {
		bandValue = ndotl;
		bandIndex = int(ndotl*float(MAX_BANDS-1));
	}

	vec2 viewSpaceUV = CalculateViewSpaceUV(fragPos, viewPos, normal);
    vec3 bandColor = GetBandColor(bandIndex, bandValue, viewSpaceUV);

	float specular = 0.0;
    if (material.shininess > 0.0) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        
        if (material.useCelShading) {
            specular = step(material.specularCutOff, specIntensity);
        } else {
            specular = specIntensity;
        }
    }
    
    vec3 ambient = dirLight.color * material.ambient * bandColor;
    vec3 diffuse = dirLight.color * bandValue * bandColor * dirLight.intensity;
    vec3 specularColor = dirLight.color * specular * sampledSpecular * material.specular;
    
    return ambient + diffuse + specularColor;
}

float CalculateAttenuation(float distance, float radius) {
    float d = max(distance, 0.0001); 
    float r = radius;
    float d2 = d * d;
    float r2 = r * r;
    return 2.0 / (d2 + r2 + d * sqrt(d2 + r2));
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 sampledDiffuse, vec3 sampledSpecular) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = CalculateAttenuation(distance, light.radius);
    
    float ndotl = max(dot(normal, lightDir), 0.0);
    
    float bandValue;
    int bandIndex;
    
    if (material.useCelShading && material.bandCount > 1) {
        bandValue = CalculateDiscreteBands(ndotl);
        bandIndex = FindBandIndex(ndotl);
    } else {
        bandValue = ndotl;
        bandIndex = int(ndotl * float(MAX_BANDS - 1));
    }
    
    vec2 viewSpaceUV = CalculateViewSpaceUV(fragPos, viewPos, normal);
    vec3 bandColor = GetBandColor(bandIndex, bandValue, viewSpaceUV);

    float specular = 0.0;
    if (material.shininess > 0.0) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        
        if (material.useCelShading) {
            specular = step(material.specularCutOff, specIntensity);
        } else {
            specular = specIntensity;
        }
    }
    
    vec3 ambient = light.color * material.ambient * bandColor;
    vec3 diffuse = light.color * bandValue * bandColor * light.intensity;
    vec3 specularColor = light.color * specular * sampledSpecular * material.specular;
    
    return (ambient + diffuse + specularColor) * attenuation;
}


vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 sampledDiffuse, vec3 sampledSpecular) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float spotIntensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    float attenuation = CalculateAttenuation(distance, light.radius);
    
    float ndotl = max(dot(normal, lightDir), 0.0);
    
    float bandValue;
    int bandIndex;
    
    if (material.useCelShading && material.bandCount > 1) {
        bandValue = CalculateDiscreteBands(ndotl);
        bandIndex = FindBandIndex(ndotl);
    } else {
        bandValue = ndotl;
        bandIndex = int(ndotl * float(MAX_BANDS - 1));
    }
    
    vec2 viewSpaceUV = CalculateViewSpaceUV(fragPos, viewPos, normal);
    vec3 bandColor = GetBandColor(bandIndex, bandValue, viewSpaceUV);
    
    float specular = 0.0;
    if (material.shininess > 0.0) {
        vec3 reflectDir = reflect(-lightDir, normal);
        float specIntensity = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        
        if (material.useCelShading) {
            specular = step(material.specularCutOff, specIntensity);
        } else {
            specular = specIntensity;
        }
    }
    
    vec3 ambient = light.color * material.ambient * bandColor;
    vec3 diffuse = light.color * bandValue * bandColor * light.intensity;
    vec3 specularColor = light.color * specular * sampledSpecular * material.specular;
    
    return (ambient + diffuse + specularColor) * spotIntensity * attenuation;
}



float CalculateOutline(vec3 normal, vec3 viewDir) {
    if (material.outlineWidth <= 0.0) return 1.0;
    
    float edge = dot(normal, viewDir);
    return smoothstep(0.0, material.outlineWidth, edge);
}
void main()
{
    vec3 norm;
    if (material.useNormalMap) {
        vec3 normalMap = texture(normal1, texCoord).rgb;
        normalMap = normalize(normalMap * 2.0 - 1.0);
        norm = normalize(TBN * normalMap);
    } else {
        norm = normalize(normal);
    }

    vec3 viewDir = normalize(viewPos - fragPos);
    
    vec3 sampledDiffuse = material.useDiffuseMap ? texture(diffuse1, texCoord).rgb : material.diffuse;
    vec3 sampledSpecular = material.useSpecularMap ? texture(specular1, texCoord).rgb : material.specular;
    vec3 emissiveColor = material.useEmissiveMap ? texture(emissive1, texCoord).rgb : vec3(0.0);
    
    vec3 result = vec3(0.0);

    if (material.useCelShading) {
        for(int i = 0; i < numDirLights; i++) {
            result += CalculateDirLight(dirLights[i], norm, viewDir, sampledDiffuse, sampledSpecular);
        }
        for(int i = 0; i < numPointLights; i++) {
            result += CalculatePointLight(pointLights[i], norm, fragPos, viewDir, sampledDiffuse, sampledSpecular);
        }
        for(int i = 0; i < numSpotLights; i++) {
            result += CalculateSpotLight(spotLights[i], norm, fragPos, viewDir, sampledDiffuse, sampledSpecular);
        }
    } else {
        for(int i = 0; i < numDirLights; i++) {
            result += CalculateDirLight(dirLights[i], norm, viewDir, sampledDiffuse, sampledSpecular);
        }
        for(int i = 0; i < numPointLights; i++) {
            result += CalculatePointLight(pointLights[i], norm, fragPos, viewDir, sampledDiffuse, sampledSpecular);
        }
        for ( int i = 0; i < numSpotLights; i++) {
            result += CalculateSpotLight(spotLights[i], norm, fragPos, viewDir, sampledDiffuse, sampledSpecular);
        }
    }

    
    if (material.useViewSpaceTexture) {
        vec2 viewUV = CalculateViewSpaceUV(fragPos, viewPos, norm);
        vec3 viewTex = texture(viewSpaceTexture, viewUV).rgb;
        result = mix(result, viewTex, 0.3);
    }
    
    float outlineMask = CalculateOutline(norm, viewDir);
    result = mix(material.outlineColor, result, outlineMask);
    
    float alpha = material.useDiffuseMap ? texture(diffuse1, texCoord).a : 1.0;
    if (hasAlpha && alpha < 0.1) discard;


    result += emissiveColor;
    FragColor = vec4(result, alpha);