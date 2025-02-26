#version 330

in vec4 vertex;
in vec3 Color;

//per frame
#include "../include/frameConstants.glsl"

out vec3 colorOUT;

void main()
{
	colorOUT = Color;
    gl_Position = frame.viewProjectionMatrix * vertex;
}