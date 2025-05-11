#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D exitTexture;
uniform vec3 color;

void main()
{
    FragColor = texture(exitTexture, TexCoord) * vec4(color, 1.0);
}