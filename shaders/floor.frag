#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // Received from vertex shader

uniform sampler2D floorTexture;
uniform bool isCeiling; // To differentiate between floor and ceiling

void main() {
    if (isCeiling) {
        // Use ceiling texture or modify floor texture
        FragColor = texture(floorTexture, TexCoords) * vec4(0.8, 0.8, 0.8, 1.0); // Slightly darker for ceiling
    } else {
        // Use floor texture
        FragColor = texture(floorTexture, TexCoords);
    }
}