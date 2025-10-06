#version 460 core
out vec4 FragColor;

void main()
{
   FragColor = vec4(1.0); // if we multiply with a vec containing color and alpha-opacity we get colored texture
}