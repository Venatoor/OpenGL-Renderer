#version 460 core
out vec4 FragColor;

in vec3 normal;
in vec3 fragPos;
in vec2 texCoord;
in mat3 TBN;
in vec4 fragPosLightSpace;

//============== STRUCT DEFINITIONS ===============

struct Material {

	vec3 albedo;
	float metallic;
	float specular;
	float ao;
	float roughness;

	float anisotropy;

	float clearcoat;
	float clearcoatRoughness;

	float sheen;
	float subsurfaceThickness;
	vec3 sheenColor;
	vec3 subsurfaceColor;

	float reflectivity;
	float refractionRatio;
	float IOR;

	bool useAlbedoMap;
    bool useMetallicMap;
    bool useSpecularMap;
    bool useRoughnessMap;
    bool useAOMap;
    bool useNormalMap;
    bool useEmissiveMap;
    bool useClearcoatMap;
    bool useSheenMap;
    bool useSubsurfaceMap;
    bool useAnisotropyMap;

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

//=================== LIGHT UNIFORMS ========================

#define MAX_DIR_LIGHT 1
#define MAX_POINT_LIGHT 16 
#define MAX_SPOT_LIGHT 16


uniform DirLight dirLights[MAX_DIR_LIGHT];
uniform PointLight pointLights[MAX_POINT_LIGHT];
uniform SpotLight spotLights[MAX_SPOT_LIGHT];

uniform int numDirLights;
uniform int numPointLights;
uniform int numSpotLights;

//=================== MATERIAL MAPS ==========================

uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D specularMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D normalMap;
uniform sampler2D emissiveMap;
uniform sampler2D clearcoatMap;
uniform sampler2D sheenMap;
uniform sampler2D subsurfaceMap;
uniform sampler2D anisotropyMap;


uniform sampler2D shadowMap;

uniform samplerCube skybox;

uniform vec3 viewPos;

uniform Material material;

uniform bool hasAlpha;

//#include "utils.glsl"


#ifndef BRDF_FUNCTIONS
#define BRDF_FUNCTIONS

#define PI 3.141592653589793
#define INV_PI 0.3183098861837907
#define EPSILON 0.00001

//======================================
// UTILITIES
//======================================

struct BxDFContext {
	
	vec3 n;
	vec3 l;
	vec3 h;
	vec3 v;

	float NdotL;
	float NdotV;
	float NdotH;
	float VdotH;
	float LdotH;

	float NdotLClamped;
	float NdotVClamped;

};

BxDFContext InitBxDFContext(vec3 worldNormal, vec3 lightDir, vec3 viewDir) {
	
	BxDFContext cx;
	cx.n = normalize(worldNormal);
	cx.l = normalize(lightDir);
	cx.v = normalize(viewDir);
	cx.h = normalize(cx.l + cx.v);

	cx.NdotL = dot(cx.n,cx.l);
	cx.NdotV = dot(cx.n,cx.v);
	cx.NdotH = dot(cx.n,cx.h);
	cx.VdotH = dot(cx.v,cx.h);
	cx.LdotH = dot(cx.l,cx.h);

	cx.NdotLClamped = max(cx.NdotL,EPSILON);
	cx.NdotVClamped = max(cx.NdotV,EPSILON);

	return cx;
};

vec3 FSchlick(vec3 F0, float cosTheta) {

	float cosThetaClamped = clamp(cosTheta, 0.0, 1.0);
	return F0 + (1.0 - F0) * pow(1.0 - cosThetaClamped, 5.0);
};

float F0(float refractionIndex) {
	
	float iorClamped = max(refractionIndex, 1.0);
	return pow(( iorClamped - 1 ) / ( iorClamped + 1),2); 
};


float DGGX(float NdotH, float roughness) {
	
	float a = max(roughness * roughness, EPSILON);
	float a2 = a * a;

	float NdotHClamped = max(NdotH,0.0);

	float denominator = 1.0 + NdotHClamped*NdotHClamped *  (a2 - 1.0);

	return a2 / max((denominator * denominator * PI),EPSILON); 

};


float GSmithComponent(float dotProduct, float roughness) {
	
	float clampedDotProduct = max(dotProduct, EPSILON);
	float nominator = 2.0 * clampedDotProduct;
	float a = roughness * roughness;
	float denominator = clampedDotProduct + sqrt(a + ( 1.0 - a) * clampedDotProduct * clampedDotProduct);
	return nominator / denominator;
};

float GSmith(float NdotV, float NdotL, float roughness) {

	return GSmithComponent(NdotV, roughness) * GSmithComponent(NdotL, roughness);
	
};

float ShlickKDisney(float roughness) {

	return pow(( roughness * roughness + 1.0),2.0) / 8.0 ;
};

float ShlickKDisneyAlternative(float roughness) {
	
	return roughness * roughness / 2.0;
};

float GShlickComponent(float k, float dotProduct) {
	
	float clampedDotProduct = max(dotProduct, EPSILON);
	float nominator = clampedDotProduct;
	float denominator = (clampedDotProduct * ( 1.0 - k ) + k);
	return nominator / denominator;
};

vec3 CookTorranceBRDF(BxDFContext cx, vec3 F0, float roughness) {
	
	float D = DGGX(cx.NdotH, roughness);
	vec3 F = FSchlick(F0, cx.VdotH);
	float G = GSmith(cx.NdotVClamped,cx.NdotLClamped,roughness);

	vec3 numerator = D * F * G;
	float denominator = 4.0 * cx.NdotVClamped * cx.NdotLClamped;

	return numerator / max(denominator, EPSILON);

};

//================================================
// DIFFUSE BRDFS
//================================================

vec3 LambertBRDF(vec3 albedo) {
	
	return max(albedo, vec3(0.0)) * INV_PI;
};

vec3 FresnelLambertBRDF(BxDFContext cx, vec3 albedo, vec3 F0) {
	
	vec3 lambertBase = LambertBRDF(albedo);
	vec3 energyTerm = 1.0 - FSchlick(F0, cx.VdotH);
	return lambertBase * energyTerm;
};

//==================================================
// DISNEY BRDF 
//==================================================

float FD90(float roughness, float HdotL) {
	
	float dotProduct = HdotL*HdotL;
	return 0.5 + 2*sqrt(roughness) * dotProduct; 

};

float DisneyDiffuseTerm(float roughness, float NdotL, float NdotV, float HdotL) {
	

	return ( 1 + ( FD90(roughness, HdotL) - 1 ) * pow(( 1 - NdotL),5)) * ( 1 + (FD90(roughness,HdotL) - 1) * pow(( 1 - NdotV),5)); 
	
};

float DisneyFSS90(float roughness, float HdotL) {

	return sqrt(roughness)* HdotL * HdotL;
};

float DisneyFSS(float roughness, float HdotL, float NdotL, float NdotV) {
	
	float firstTerm = ( 1 + (DisneyFSS90(roughness,HdotL) - 1) * pow(( 1 - NdotL),5) );
	float secondTerm = ( 1 + (DisneyFSS90(roughness,HdotL) - 1) * pow((1 - NdotV),5) );

	return  firstTerm * secondTerm;
};

vec3 DisneyBurleyBSDF(vec3 albedo, float roughness, float kSS, BxDFContext cx ) {
	
	vec3 lambertTerm = LambertBRDF(albedo);
	float subsurfaceTerm = 1.25 * kSS * DisneyFSS(roughness, cx.LdotH, cx.NdotLClamped, cx.NdotVClamped);
	float diffuseTerm = ( 1 - kSS ) * DisneyDiffuseTerm(roughness, cx.NdotL, cx.NdotV, cx.LdotH);

	return lambertTerm * ( subsurfaceTerm + diffuseTerm );
};

// next cloth, anisotropic ggx, cook torrance, thin film, clear coat, sss

//=========================================
// ANISOTROPIC PARAMETRIZATION + BRDF 
//=========================================


void GetAnisotropicRoughness(float roughness, float anisotropy, out float ax, out float ay) {
	
	float alpha = max(roughness * roughness, EPSILON);

	float aspect = sqrt(1.0 - 0.9 * abs(anisotropy));

	ax = max(alpha / aspect, EPSILON);
	ay = max(alpha * aspect, EPSILON);

}

float DGGXAnisotropic(float NdotH, float TdotH, float BdotH, float ax, float ay) {

	float denominator = PI * ax * ay * pow( (TdotH * TdotH)/(ax*ax) + (BdotH * BdotH)/(ay*ay) + NdotH*NdotH,2);
	return 1.0 / max(denominator,EPSILON);

}


//==========================================
// SHEEN
//==========================================


float CharlieNDF(float NdotH, float roughness) {

	float alpha = max(roughness * roughness, EPSILON);
	float invAlpha = 1.0 / alpha;

	float sinTheta = 1.0 - NdotH * NdotH;
	return (1 / (2 * PI)) * (2 + invAlpha) * pow(sinTheta,invAlpha);

};

vec3 BRDF_Sheen(BxDFContext cx, vec3 sheenColor, float sheenRoughness) {
	
	vec3 F = FSchlick(sheenColor, cx.LdotH);
	float D = CharlieNDF(cx.NdotH, sheenRoughness);

	return F * D * INV_PI;
};


//===================================
//CLEARCOAT
//===================================

vec3 BRDF_Clearcoat(BxDFContext cx, float strength, float roughness) {

	vec3 F0_clearcoat = vec3(0.04);
	vec3 F = FSchlick(F0_clearcoat, cx.LdotH);

	float D = DGGX(cx.NdotH, roughness);

	float G = GSmith(cx.NdotVClamped, cx.NdotLClamped, roughness);

	vec3 numerator = F * D * G;
	float denominator = 4.0 * cx.NdotVClamped * cx.NdotLClamped;
	return ( numerator / max(denominator, EPSILON)) * strength;

};

//=====================================
// ANISOTROPIC SPECULAR BRDF
//=====================================

vec3 BRDF_Specular_Anisotropic(BxDFContext cx, vec3 tangent, vec3 bitangent, vec3 F0, float ax, float ay) {

	vec3 F = FSchlick(F0, cx.LdotH);

	float TdotH = dot(tangent, cx.h);
	float BdotH = dot(bitangent, cx.h);
	float D = DGGXAnisotropic(cx.NdotH,TdotH,BdotH,ax,ay);

	float avgRoughness = sqrt(ax*ay);
	float G = GSmith(cx.NdotVClamped, cx.NdotLClamped, avgRoughness);

	vec3 numerator = F * G * D;
	float denominator = 4.0 * cx.NdotVClamped * cx.NdotLClamped;
	return numerator / max(denominator, 0.0001);

};


#endif

//============= MATERIAL PROPERTY CALCULATIONS ================

void GetMaterialProperties(out vec3 albedo, out float metallic, out float specular, 
						   out float roughness, out float ao, out float clearcoat,out float clearcoatRoughness ,
						   out float sheen, out vec3 sheenColor, out vec3 subsurfaceColor,
						   out float subsurfaceThickness, out float anisotropy, out vec3 anisotropyDirection) 
						   {

	albedo = material.useAlbedoMap ? texture(albedoMap, texCoord).rgb : material.albedo;
	metallic = material.useMetallicMap ? texture(metallicMap, texCoord).r : material.metallic;
	specular = material.useSpecularMap ? texture(specularMap, texCoord).r : material.specular;
	roughness = material.useRoughnessMap ? texture(roughnessMap, texCoord).r : material.roughness;
	ao = material.useAOMap ? texture(aoMap, texCoord).r : material.ao;

	clearcoat = material.useClearcoatMap ? texture(clearcoatMap, texCoord).r : material.clearcoat;
	clearcoatRoughness = material.clearcoatRoughness;

	sheen = material.useSheenMap ? texture(sheenMap, texCoord).r : material.sheen;
	sheenColor = material.sheenColor;

	subsurfaceColor = material.useSubsurfaceMap ? texture(subsurfaceMap, texCoord).rgb : material.subsurfaceColor;
    subsurfaceThickness = material.subsurfaceThickness;
    
    anisotropy = material.anisotropy;
	if (abs(anisotropy) > 0.01) {
		anisotropyDirection = normalize(TBN[0]); // Use tangent as anisotropy direction
	}	

	metallic = clamp(metallic, 0.0, 1.0);
    specular = clamp(specular, 0.0, 1.0);
    roughness = max(roughness, 0.001);
    clearcoat = clamp(clearcoat, 0.0, 1.0);
    sheen = clamp(sheen, 0.0, 1.0);
    anisotropy = clamp(anisotropy, -1.0, 1.0);
}

vec3 CalculateLightContribution( vec3 lightColor, float intensity, vec3 lightDir, BxDFContext cx, 
                               vec3 F0, vec3 albedo, float roughness, float metallic,
                               float specular, float clearcoat, float clearcoatRoughness,
                               float sheen, vec3 sheenColor, float sheenRoughness,
                               vec3 subsurfaceColor, float subsurfaceThickness,
                               float anisotropy, vec3 anisotropyDirection) {

	vec3 diffuseColor = mix(albedo, vec3(0.0), metallic);

	vec3 diffuseBRDF;
	if ( subsurfaceThickness > 0.01 ) {
		
		diffuseBRDF = DisneyBurleyBSDF(albedo, roughness, subsurfaceThickness, cx);

	}
	else {
		diffuseBRDF = FresnelLambertBRDF(cx, albedo, F0);
	}

	//specular calculations now 

	vec3 specularBRDF = CookTorranceBRDF(cx, F0, roughness);
	vec3 anisotropicBRDF = vec3(0.0);
	if ( abs(anisotropy) > 0.01 ) {
		vec3 tangent = normalize(anisotropyDirection);
		vec3 bitangent = normalize(cross(tangent,cx.n));
		float ax, ay;
		GetAnisotropicRoughness(roughness, anisotropy, ax, ay);
		anisotropicBRDF = BRDF_Specular_Anisotropic(cx, tangent, bitangent, F0, ax, ay);
	}

	vec3 finalSpecular = ( anisotropicBRDF != vec3(0.0)) ? anisotropicBRDF : specularBRDF;

	//sheen component
	vec3 sheenBRDF = vec3(0.0);
	if ( sheen > 0.01 ) {
		sheenBRDF = BRDF_Sheen(cx, sheenColor, sheenRoughness) * sheen;
	}

	//clearcoat component
	vec3 clearcoatBRDF = vec3(0.0);
	if ( clearcoat > 0.01) {
		clearcoatBRDF = BRDF_Clearcoat(cx, clearcoat, clearcoatRoughness);
	}

	// energy conservation

	vec3 kS = FSchlick(F0, max(dot(cx.h,cx.v), 0.0));
	vec3 kD = (( 1.0 - kS ) * ( 1.0 - metallic ));

	diffuseBRDF *= kD;

	// the output of the rendering equation for a single dOmega contribution of the hemisphere

	vec3 radiance = (diffuseBRDF + finalSpecular + sheenBRDF + clearcoatBRDF) * lightColor * intensity * cx.NdotLClamped;
	return radiance;
}

float CalculateAttenuation(float distance, float radius) {
    float d = max(distance, 0.0001); // Avoid division by zero
    float r = radius;
    float d2 = d * d;
    float r2 = r * r;
    return 2.0 / (d2 + r2 + d * sqrt(d2 + r2));
}


void main() {

	vec3 norm;
    if ( material.useNormalMap ) {

        vec3 normalMap = texture(normalMap, texCoord).rgb;
        normalMap = normalize(normalMap * 2.0 - 1.0);
        norm = normalize(TBN * normalMap);
    }
    else {
        norm = normalize(normal);
    }


	vec3 viewDir = normalize(viewPos - fragPos);

	vec3 albedo;
    float metallic, specular, roughness, ao;
    float clearcoat, clearcoatRoughness;
    float sheen;
    vec3 sheenColor, subsurfaceColor;
    float subsurfaceThickness, anisotropy;
    vec3 anisotropyDirection;
    
    GetMaterialProperties(albedo, metallic, specular, roughness, ao, clearcoat,
                         clearcoatRoughness, sheen, sheenColor, subsurfaceColor,
                         subsurfaceThickness, anisotropy, anisotropyDirection);

	vec3 F0 = mix(vec3(0.04),albedo,metallic);

	vec3 result = vec3(0.0);

	// here i am calculating DIRECTIONAL LIGHTS CONTRIBUTIONS
	for(int i = 0; i < numDirLights; i++) {
        vec3 lightDir = normalize(-dirLights[i].direction);
        BxDFContext cx = InitBxDFContext(norm, lightDir, viewDir);
        
        result += CalculateLightContribution(dirLights[i].color, dirLights[i].intensity,
                                           lightDir, cx, F0, albedo, roughness,
                                           metallic, specular, clearcoat, clearcoatRoughness,
                                           sheen, sheenColor, roughness, subsurfaceColor,
                                           subsurfaceThickness, anisotropy, anisotropyDirection);
    }
	
	// POINT LIGHTS

	for(int i = 0; i < numPointLights; i++) {
        vec3 lightDir = normalize(pointLights[i].position - fragPos);
        BxDFContext cx = InitBxDFContext(norm, lightDir, viewDir);
        float distance = length(pointLights[i].position - fragPos);
        float attenuation = CalculateAttenuation(distance, pointLights[i].radius);
        
        vec3 contribution = CalculateLightContribution(pointLights[i].color, pointLights[i].intensity,
                                                     lightDir, cx, F0, albedo, roughness,
                                                     metallic, specular, clearcoat, clearcoatRoughness,
                                                     sheen, sheenColor, roughness, subsurfaceColor,
                                                     subsurfaceThickness, anisotropy, anisotropyDirection);
        result += contribution * attenuation;
    }

	for(int i = 0; i < numSpotLights; i++) {
		vec3 lightDir = normalize(spotLights[i].position - fragPos);
		BxDFContext cx = InitBxDFContext(norm, lightDir, viewDir);
		float distance = length(spotLights[i].position - fragPos);
    
		// Spotlight intensity (soft edges)
		float theta = dot(lightDir, normalize(-spotLights[i].direction));
		float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
		float intensity = clamp((theta - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
    
		float attenuation = CalculateAttenuation(distance, spotLights[i].radius);
    
		vec3 contribution = CalculateLightContribution(spotLights[i].color, spotLights[i].intensity,
													 lightDir, cx, F0, albedo, roughness,
													 metallic, specular, clearcoat, clearcoatRoughness,
													 sheen, sheenColor, roughness, subsurfaceColor,
													 subsurfaceThickness, anisotropy, anisotropyDirection);
    
		result += contribution * intensity * attenuation;
	}

	float alpha = material.useAlbedoMap ? texture(albedoMap, texCoord).a : 1.0;

    if (hasAlpha && alpha < 0.1) discard;

    if (alpha > 0.95 && (material.reflectivity > 0.0 || material.IOR != 1.0)) {
        // Calculate proper Fresnel using PBR F0
        vec3 F = FSchlick(F0, max(dot(norm, viewDir), 0.0));
        float fresnel = length(F); // Get magnitude for mixing
        
        if (material.reflectivity > 0.0) {
            vec3 reflectDir = reflect(-viewDir, norm);
            // Use roughness to select mipmap level for blurry reflections
            float lodLevel = roughness * 4.0;
            vec3 reflection = textureLod(skybox, reflectDir, lodLevel).rgb;
            
            // Apply reflectivity and energy conservation
            result = mix(result, reflection, fresnel * material.reflectivity);
        }
        
        if (material.IOR != 1.0) {
            // Calculate proper refraction ratio (air to material)
            float eta = 1.0 / material.IOR;
            vec3 refractDir = refract(-viewDir, norm, eta);
            
            if (length(refractDir) > 0.0) { // Check for total internal reflection
                // Use roughness for blurry refraction
                float lodLevel = roughness * 4.0;
                vec3 refraction = textureLod(skybox, refractDir, lodLevel).rgb;
                
                // Apply Beer-Lambert law for colored absorption through material
                float depth = 0.1; // Approximate thickness
                vec3 absorption = exp(-subsurfaceColor * depth);
                refraction *= absorption;
                
                // Apply refraction with energy conservation
                result = mix(result, refraction, (1.0 - fresnel) * material.reflectivity);
            }
        }
    }

	// Emissive component
    if (material.useEmissiveMap) {
        result += texture(emissiveMap, texCoord).rgb;
    }

	result *= ao;
    
    FragColor = vec4(result, alpha);
}