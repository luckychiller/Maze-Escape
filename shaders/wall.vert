#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;    // Vertex normal from VBO

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;       // Fragment position in world space
out vec3 Normal;        // Normal in world space
out vec2 TexCoords;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0)); // Transform vertex position to world space
    
    // Transform normal to world space. Use inverse transpose for non-uniform scaling.
    // mat3(transpose(inverse(model))) is the normal matrix.
    Normal = mat3(transpose(inverse(model))) * aNormal; 
    TexCoords = aTexCoords;
}