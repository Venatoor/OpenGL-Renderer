#version 460

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