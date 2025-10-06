#version 460 core

layout ( location = 0) out vec3 gPosition;
layout ( location = 1 ) out vec3 gNormal;
layout ( location = 2 ) out vec4 gAlbedo; // ( rgb = color && a = alpha ) 
layout ( location = 3 ) out int gShadingModel;
layout ( location = 4 ) out vec4 gSpecular; // ( rgb = color && a = shininess ) 
layout ( location = 5 ) out vec3 gMaterial; // ( r = ambient, g = reflection, b = refraction)

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
in mat3 TBN;
in vec4 fragPosLightSpace;

//============== CONSTANTS ===============================

const float REFRACTION_THRESHOLD = 1.0001;
const float REFLECTION_THRESHOLD = 0.0001;

//============== STRUCT DEFINITIONS ======================

// Materials

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

// Samplers 

uniform	sampler2D diffuse1;
uniform	sampler2D specular1;
uniform	sampler2D emissive1;
uniform sampler2D normal1;

uniform sampler2D shadowMap;

// Material Properties

uniform vec3 viewPos;
uniform Material material;
uniform bool hasAlpha;

uniform int shadingModelID;

// Additional Stuff

uniform samplerCube skybox;


float fresnelSchlick(float cosTheta, float F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {

	float alpha = material.useDiffuseMap ? texture(diffuse1, texCoord).a : 1.0;
    if (hasAlpha && alpha < 0.1) discard;
	
	vec3 norm;
    if ( material.useNormalMap ) {

        vec3 normalMap = texture(normal1, texCoord).rgb;
        normalMap = normalize(normalMap * 2.0 - 1.0);
        norm = normalize(TBN * normalMap);
    }
    else {
        norm = normalize(normal);
    }

	/*
    vec3 sampledDiffuse = vec3(1.0);
	if ( material.useDiffuseMap ) {
		 sampledDiffuse = vec3(texture(diffuse1, texCoord));
	}
	*/

	vec3 sampledDiffuse = material.useDiffuseMap ? texture(diffuse1, texCoord).rgb : vec3(1.0);

	/*
	vec3 sampledSpecular = vec3(1.0);
	if ( material.useSpecularMap ) {
		sampledSpecular = vec3(texture(specular1, texCoord));
	}
	*/

	vec3 sampledSpecular = material.useSpecularMap ? texture(specular1, texCoord).rgb : vec3(1.0);
	vec3 emissiveColor = material.useEmissiveMap ? texture(emissive1, texCoord).rgb : vec3(0.0);

	/*
	vec3 emissiveColor = vec3(0.0);
	if(material.useEmissiveMap) {
		emissiveColor = vec3(texture(emissive1, texCoord));
	}
	*/
	vec3 finalSpecular = material.specular * sampledSpecular;
	vec3 baseColor = sampledDiffuse + emissiveColor;

	if (alpha > 0.95) {
        vec3 viewDir = normalize(viewPos - fragPos);
        float cosTheta = max(dot(viewDir, norm), 0.0);
        float F0 = 0.04;
        float fresnel = fresnelSchlick(cosTheta, F0);

        if (material.reflectivity > REFLECTION_THRESHOLD) {
            vec3 reflectDir = reflect(-viewDir, norm);
            vec3 reflection = texture(skybox, reflectDir).rgb;
            baseColor = mix(baseColor, reflection, fresnel * material.reflectivity);
        }

        if (material.refractionRatio > REFRACTION_THRESHOLD) {
            float ratio = 1.0 / material.refractionRatio;
            vec3 refractDir = refract(-viewDir, norm, ratio);
            vec3 refraction = texture(skybox, refractDir).rgb;
            baseColor = mix(baseColor, refraction, (1.0 - fresnel) * material.reflectivity);
        }
    }

	gPosition = fragPos;
	gNormal = norm;
	gAlbedo = vec4(baseColor, alpha);
	gSpecular = vec4(finalSpecular, material.shininess);
	gShadingModel = shadingModelID;
	gMaterial = vec3(material.ambient, material.reflectivity, material.refractionRatio);
	
	
}
