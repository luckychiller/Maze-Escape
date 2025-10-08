#version 330 core
out vec4 FragColor;

in vec3 TexCoords; // Direction vector from camera to cube vertex

uniform samplerCube skybox; // Cubemap sampler

void main() {    
    FragColor = texture(skybox, TexCoords);
}
