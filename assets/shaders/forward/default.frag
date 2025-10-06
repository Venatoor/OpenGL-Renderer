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

uniform sampler2D shadowMap;

uniform samplerCube skybox;

uniform vec3 viewPos;

uniform Material material;

uniform bool hasAlpha;

#include "utils.glsl"   

float CalculateShadowsDirectionalLight(vec4 fragPosLightSpace, vec3 lightDir, vec3 normal) {
    
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float shadowDepth = texture(shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
    float shadow = currentDepth - bias > shadowDepth ? 1.0 : 0.0;

    // Check if fragment is outside light frustum
    if(projCoords.z > 1.0 ) {
        return 0.0;
    }

    return shadow;

}

vec3 CalculateDirLight(DirLight dirLight, vec3 normal, vec3 viewDir, vec3 sampledDiffuse, 
                       vec3 sampledSpecular) {

	vec3 lightDir = normalize(-dirLight.direction);

	float diff = max(dot(normal, lightDir),0.0);

	vec3 halfwayDir = normalize(viewDir + lightDir);
	float spec = pow(max(dot(normal,halfwayDir),0.0), material.shininess);

	vec3 ambient = dirLight.color * material.ambient * sampledDiffuse;
	vec3 diffuse = dirLight.color * diff * sampledDiffuse * dirLight.intensity;
	vec3 specular = dirLight.color * spec * sampledSpecular * material.specular * dirLight.intensity;
	return ( ambient + diffuse + specular );

}



float CalculateAttenuation(float distance, float radius) {
    float d = max(distance, 0.0001); // Avoid division by zero
    float r = radius;
    float d2 = d * d;
    float r2 = r * r;
    return 2.0 / (d2 + r2 + d * sqrt(d2 + r2));
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 sampledDiffuse, vec3 sampledSpecular) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = CalculateAttenuation(distance, light.radius);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    vec3 ambient = light.color * material.ambient * sampledDiffuse;
    vec3 diffuse = light.color * diff * sampledDiffuse * light.intensity;
    vec3 specular = light.color * spec * sampledSpecular * material.specular * light.intensity;
    
    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 sampledDiffuse, vec3 sampledSpecular) {
    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    
    // Spotlight intensity (soft edges)
    float theta = dot(normalize(-lightDir), normalize(light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    float attenuation = CalculateAttenuation(distance, light.radius);

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    float shadow = CalculateShadowsDirectionalLight(fragPosLightSpace, lightDir, normal);

    // Combine results
    vec3 ambient = light.color * material.ambient * sampledDiffuse;
    vec3 diffuse = light.color * diff * sampledDiffuse * light.intensity;
    vec3 specular = light.color * spec * sampledSpecular * material.specular * light.intensity;
    
    return (ambient + (diffuse + specular)) * intensity * attenuation;
}

float fresnelSchlick(float cosTheta, float F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{

	//NORMALISE VECTORS
	vec3 norm;
    if ( material.useNormalMap ) {

        vec3 normalMap = texture(normal1, texCoord).rgb;
        normalMap = normalize(normalMap * 2.0 - 1.0);
        norm = normalize(TBN * normalMap);
    }
    else {
        norm = normalize(normal);
    }


	vec3 viewDir = normalize(viewPos - fragPos);
	//vec3 reflectDir = reflect(-lightDir, norm);

	//Texture samples
	vec3 sampledDiffuse = vec3(1.0);
	if ( material.useDiffuseMap ) {
		 sampledDiffuse = vec3(texture(diffuse1, texCoord));
	}
	vec3 sampledSpecular = vec3(1.0);
	if ( material.useSpecularMap ) {
		sampledSpecular = vec3(texture(specular1, texCoord));
	}
	vec3 emissiveColor = vec3(0.0);
	if(material.useEmissiveMap) {
		emissiveColor = vec3(texture(emissive1, texCoord));
	}	

	vec3 result = vec3(0.0f);

	// Directional lights
    for(int i = 0; i < numDirLights; i++) {
        result += CalculateDirLight(dirLights[i], norm, viewDir, sampledDiffuse, sampledSpecular);
    }
    
    // Point lights
    for(int i = 0; i < numPointLights; i++)
        result += CalculatePointLight(pointLights[i], norm, fragPos, viewDir, sampledDiffuse, sampledSpecular);
    
    // Spot lights
    for(int i = 0; i < numSpotLights; i++)
        result += CalculateSpotLight(spotLights[i], norm, fragPos, viewDir, sampledDiffuse, sampledSpecular);
    
    
    float alpha = material.useDiffuseMap ? texture(diffuse1, texCoord).a : 1.0;
    if (hasAlpha && alpha < 0.1) discard;

    if ( alpha > 0.95 ) {
        if ( material.useReflection || material.useRefraction) {

        float cosTheta = max(dot(viewDir, norm), 0.0);
        float F0 = 0.04; // Base reflectivity for dielectrics
        float fresnel = fresnelSchlick(cosTheta, F0);

            if ( material.useReflection) {

                vec3 reflectDir = reflect(viewDir, norm);
                vec3 reflection = texture(skybox, reflectDir).rgb;
                result = mix(result,reflection,fresnel * material.reflectivity);
            }

            if (material.useRefraction) {
                float ratio = 1.0 / material.refractionRatio;
                vec3 refractDir = refract(viewDir, norm, ratio);
                vec3 refraction = texture(skybox, refractDir).rgb;

                result = mix(result, refraction, ( 1.0 - fresnel ) * material.reflectivity);
            }
        }

    }

    //adding emissive component last

    result += emissiveColor;

    FragColor = vec4(result, alpha);
}