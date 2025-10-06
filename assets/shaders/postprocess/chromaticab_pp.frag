#version 460 core
out vec4 FragColor;

in vec2 TexCoords;	

uniform sampler2D scene;
uniform bool grayscale;

uniform bool applyGamma = true;

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

void main() {

	vec4 color = texture(scene, TexCoords);

	ApplyChromaticAberration(color, scene, TexCoords);

	if (applyGamma) {
        color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
    }

	FragColor = color;
}