#version 460 core

layout(location = 0) in vec3 aPos;

uniform mat4 lightViewProj;
uniform mat4 model;

void main() {

	vec4 worldPos = model * vec4(aPos, 1.0);
	gl_Position = lightViewProj * worldPos;
}