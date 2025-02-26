#version 400 
layout(location = 0) in vec4 position;
layout(location = 2) in vec2 texCoords;

out vec2 TexCoords;


void main()
{
    gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
    TexCoords = texCoords;
}