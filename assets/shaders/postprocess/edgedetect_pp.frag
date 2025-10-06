#version 460 core
out vec4 FragColor;

in vec2 TexCoords;	

uniform sampler2D scene;
uniform bool grayscale;

uniform bool applyGamma = true;

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

void main() {

	vec4 color = texture(scene, TexCoords);

	ApplyEdgeDetection(color, scene, TexCoords);

	if (applyGamma) {
        color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
    }

	FragColor = color;
}