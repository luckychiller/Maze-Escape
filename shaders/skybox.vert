#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords; // Will be the vertex position, used as direction vector

uniform mat4 projection;
uniform mat4 view; // View matrix without translation

void main() {
    TexCoords = aPos; // Pass vertex position directly as texture coordinate for cubemap
    
    // Remove translation from the view matrix to make skybox follow camera
    mat4 viewNoTranslation = mat4(mat3(view)); 
    vec4 pos = projection * viewNoTranslation * vec4(aPos, 1.0);
    
    // Ensure skybox is always at the far plane (depth = 1.0)
    // This makes it render behind everything else.
    gl_Position = pos.xyww; 
}
