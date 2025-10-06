#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D shadowMap;

void main() {
    float depthValue = texture(shadowMap, TexCoords).r;
    FragColor = vec4(vec3(depthValue), 1.0);
    
    // Optional: Enhance contrast for better visualization
    // FragColor = vec4(vec3(pow(depthValue, 10.0)), 1.0);
}