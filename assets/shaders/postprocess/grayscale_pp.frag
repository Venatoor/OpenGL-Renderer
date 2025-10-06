#version 460 core
out vec4 FragColor;

in vec2 TexCoords;	

uniform sampler2D scene;
uniform bool grayscale;

uniform bool applyGamma = true;

void ApplyGrayscale(inout vec4 color) {

	float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
	color.rgb = vec3(average);
}

void main() {

	vec4 color = texture(scene, TexCoords);

	ApplyGrayscale(color);

	if (applyGamma) {
        color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
    }

	FragColor = color;
}