#version 460 core
out vec4 FragColor;

in vec2 TexCoords;	

uniform sampler2D scene;
uniform bool grayscale;

uniform bool applyGamma = true;

void ApplyInvert(inout vec4 color) {
	color.rgb = 1 - color.rgb;
}

void main() {

	vec4 color = texture(scene, TexCoords);

	ApplyInvert(color);

	if (applyGamma) {
        color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
    }

	FragColor = color;
}