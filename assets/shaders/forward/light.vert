#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;

uniform mat4 model;
uniform mat4 camMatrix;

void main()
{
   gl_Position = camMatrix * model * vec4(aPos, 1.0f);
   // gl_Position for scale :gl_Position = vec4(aPos.x + aPos.x * scale, aPos.y + aPos.y * scale, aPos.z + aPos.z * scale, 1.0);
}