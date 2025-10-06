#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTex;

layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;
out vec4 fragPosLightSpace;

out mat3 TBN;

uniform mat4 model;
uniform mat4 camMatrix;

uniform mat4 lightViewProj;

void main()
{
   vec4 worldPos = model * vec4(aPos, 1.0f); // model -> world position transformation
   fragPos = worldPos.xyz; // getting world pos without homogenous coordinates

   normal = mat3(transpose(inverse(model))) * aNormal; // transforming normals through a normal matrix

   vec3 T = normalize(mat3(model) * aTangent);
   vec3 B = normalize(mat3(model) * aBitangent);
   vec3 N = normalize(mat3(model) * aNormal);
   TBN = mat3(T,B,N);
	

   texCoord = aTex;

   fragPosLightSpace = lightViewProj * vec4(fragPos, 1.0);
   gl_Position = camMatrix * worldPos; // moving from world to view through mvp transform & perspective
}