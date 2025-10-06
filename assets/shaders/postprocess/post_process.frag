#version 460 core
out vec4 FragColor;

in vec2 TexCoords;	

uniform sampler2D scene;
uniform bool grayscale;

uniform bool applyGamma = true;

#define EFFECT_NONE 0
#define EFFECT_INVERT (uint(1<<0))
#define EFFECT_GRAYSCALE (uint(1<<1))
#define EFFECT_EDGEDETECT (uint(1<<2))
#define EFFECT_CHROMATICAB (uint(1<<3))
#define EFFECT_BLUR (uint(1<<4))

uniform uint activeEffects;
//perhaps later i might add more uniforms to control inner parameters of each apply function :3

void ApplyGrayscale(inout vec4 color) {

	float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	color.rgb = vec3(average);
}

void ApplyInvert(inout vec4 color) {
	color.rgb = 1 - color.rgb;
}

void ApplyEdgeDetection(inout vec4 color, sampler2D tex, vec2 texCoords){
	
	vec2 texelSize = 1.0 / textureSize(tex,0);
	float kernel[9] = float[](
		-1, -1, -1,
		-1, 8, -1,
		-1, -1, -1
	);

	vec3 sampleTex[9];
	for ( int i =0; i < 3; i++) {
		for ( int j =0; j < 3; j++) {
			sampleTex[i*3+j] = texture(tex, texCoords + vec2((i-1)*texelSize.x,(j-1)*texelSize.y)).rgb;
		}
	}

	vec3 col = vec3(0.0);
	for ( int i = 0; i < 9; i++) {
		col += sampleTex[i]*kernel[i];
	}

	color.rgb = mix(color.rgb,col,0.5);
}

void ApplyChromaticAberration(inout vec4 color, sampler2D tex, vec2 texCoords) {
	
	float amount = 0.005;
	vec2 direction = (texCoords - 0.5) * 2.0;
	float dist = length(direction);

	vec3 aberration;
	aberration.r = texture(tex, texCoords - direction * amount * 1.0).r;
	aberration.g = texture(tex, texCoords - direction * amount * 0.5).g;
	aberration.b = texture(tex, texCoords).b;

	color.rgb = mix(color.rgb, aberration, min(dist * 2.0, 1.0));

}

void ApplyBlur(inout vec4 color, sampler2D tex, vec2 texCoords) {
	
	vec2 texelSize = 1.0 / textureSize(tex,0);
	float weights[9] = float[](
        0.077847, 0.123317, 0.077847,
        0.123317, 0.195346, 0.123317,
        0.077847, 0.123317, 0.077847
    );

	vec3 result = vec3(0.0);
    for(int i = -1; i <= 1; ++i) {
        for(int j = -1; j <= 1; ++j) {
            int index = (i+1)*3 + (j+1);
            result += texture(tex, texCoords + vec2(i, j) * texelSize).rgb * weights[index];
        }
    }
    color.rgb = result;

}
 
void main() {

	vec4 color = texture(scene, TexCoords);

	if ((activeEffects & EFFECT_INVERT) != uint(0)) {
        ApplyInvert(color);
    }
    if ((activeEffects & EFFECT_GRAYSCALE) != uint(0)) {
        ApplyGrayscale(color);
    }
    if ((activeEffects & EFFECT_EDGEDETECT) != uint(0)) {
        ApplyEdgeDetection(color, scene, TexCoords);
    }
    if ((activeEffects & EFFECT_CHROMATICAB) != uint(0)) {
        ApplyChromaticAberration(color, scene, TexCoords);
    }
    if ((activeEffects & EFFECT_BLUR) != uint(0)) {
        ApplyBlur(color, scene, TexCoords);
    }

	if (applyGamma) {
        color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
    }

	FragColor = color;
}