#version 460 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gShadingModel;
uniform sampler2D gSpecular;
uniform sampler2D gMaterial;

//=================== COMMON UNIFORMS ================

uniform vec3 viewPos;
uniform sampler2D shadowMap;

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


//=================== LIGHT UNIFORMS =================


#define MAX_DIR_LIGHT 1
#define MAX_POINT_LIGHT 16 
#define MAX_SPOT_LIGHT 16


uniform DirLight dirLights[MAX_DIR_LIGHT];
uniform PointLight pointLights[MAX_POINT_LIGHT];
uniform SpotLight spotLights[MAX_SPOT_LIGHT];

uniform int numDirLights;
uniform int numPointLights;
uniform int numSpotLights;


//======================= PHONG BLINN FUNCTIONS ===============================


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
                       vec3 materialSpecular, float shininess, float materialAmbient) {

	vec3 lightDir = normalize(-dirLight.direction);

	float diff = max(dot(normal, lightDir),0.0);

	vec3 halfwayDir = normalize(viewDir + lightDir);
	float spec = pow(max(dot(normal,halfwayDir),0.0), shininess);

	vec3 ambient = dirLight.color * materialAmbient * sampledDiffuse;
	vec3 diffuse = dirLight.color * diff * sampledDiffuse * dirLight.intensity;
	vec3 specular = dirLight.color * spec * materialSpecular * dirLight.intensity;
	return ( ambient + diffuse + specular );

}



float CalculateAttenuation(float distance, float radius) {
    float d = max(distance, 0.0001); // Avoid division by zero
    float r = radius;
    float d2 = d * d;
    float r2 = r * r;
    return 2.0 / (d2 + r2 + d * sqrt(d2 + r2));
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 sampledDiffuse, vec3 materialSpecular,
    float shininess, float materialAmbient) {

    vec3 lightDir = normalize(light.position - fragPos);
    float distance = length(light.position - fragPos);
    float attenuation = CalculateAttenuation(distance, light.radius);
    
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    
    vec3 ambient = light.color * materialAmbient * sampledDiffuse;
    vec3 diffuse = light.color * diff * sampledDiffuse * light.intensity;
    vec3 specular = light.color * spec * materialSpecular  * light.intensity;
    
    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalculateSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 sampledDiffuse, vec3 materialSpecular, 
                       float shininess, float materialAmbient) {
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
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    
    //float shadow = CalculateShadowsDirectionalLight(fragPosLightSpace, lightDir, normal);

    // Combine results
    vec3 ambient = light.color * materialAmbient * sampledDiffuse;
    vec3 diffuse = light.color * diff * sampledDiffuse * light.intensity;
    vec3 specular = light.color * spec * materialSpecular * light.intensity;
    
    return (ambient + (diffuse + specular)) * intensity * attenuation;
}

float fresnelSchlick(float cosTheta, float F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
    
    vec3 fragPos = texture(gPosition, TexCoords).rgb;
    vec3 viewDir = normalize(viewPos - fragPos);

    //Setup Textures 

    vec4 albedoData = texture(gAlbedo, TexCoords);
    vec4 specularData = texture(gSpecular, TexCoords);

    vec3 norm = texture(gNormal, TexCoords).rgb;
    vec3 sampledDiffuse = albedoData.rgb;
    float sampledShininess = specularData.a;
    vec3 sampledSpecularColor = specularData.rgb;
    float alpha = albedoData.a;

    vec3 materialData = texture(gMaterial, TexCoords).rgb;
    float sampledAmbient = materialData.r;

    vec3 lighting = vec3(0.0f);


    // Directional lights
    for(int i = 0; i < numDirLights; i++) {
        lighting += CalculateDirLight(dirLights[i], norm, viewDir, 
                                      sampledDiffuse, sampledSpecularColor, sampledShininess, sampledAmbient );
    }
    
    // Point lights
    for(int i = 0; i < numPointLights; i++) {
        lighting += CalculatePointLight(pointLights[i], norm, fragPos, viewDir, 
                                        sampledDiffuse, sampledSpecularColor, sampledShininess, sampledAmbient);
    }
    
    // Spot lights
    for(int i = 0; i < numSpotLights; i++) {
        lighting += CalculateSpotLight(spotLights[i], norm, fragPos, viewDir,
                                       sampledDiffuse, sampledSpecularColor, sampledShininess, sampledAmbient);
    }


    FragColor = vec4(lighting, alpha);
}