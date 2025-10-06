
#ifndef UTILS_FUNCTIONS
#define UTILS_FUNCTIONS

// Before using this function, i should remember using TBN to convert the vectors and normalise them 

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir, sampler2D depthMap, float heightScale, float numLayers) {

	float height = texture(depthMap, texCoords).r;

	float layerDepth = 1 / numLayers;
	float currentLayerDepth = 0.0;

	vec2 p =  ( viewDir.xy / viewDir.z ) * height;
	vec2 deltaTexCoords = p / numLayers;

	vec2 currentTexCoords = texCoords;
	float currentDepthMapValue = texture(depthMap, currentTexCoords).r;

	// Step Part

	while ( currentLayerDepth < currentDepthMapValue ) {

		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = texture(depthMap, currentTexCoords).r;
		currentLayerDepth += layerDepth;


	}

	//POM PART ?

	vec2 prevTexCoord = currentTexCoords + deltaTexCoords;

	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(depthMap, prevTexCoord).r - currentLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoord * weight + currentTexCoords * (1.0 - weight);

	vec2 adjustedTexCoords = finalTexCoords;
	if ( adjustedTexCoords.x > 1 || adjustedTexCoords.y > 1 || adjustedTexCoords.x < 0 || adjustedTexCoords.y < 0 ) {
		discard;
    }
	return adjustedTexCoords;

};


#endif