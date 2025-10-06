#version 460 core
out vec4 FragColor;

in vec2 TexCoords;	

uniform sampler2D scene;
uniform bool grayscale;

uniform bool applyGamma = true;

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

    ApplyBlur(color, scene, TexCoords);

	if (applyGamma) {
        color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
    }

	FragColor = color;
}