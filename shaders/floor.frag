#version 330 core
out vec4 FragColor;

in vec3 FragPos;   // Interpolated fragment position in world space
in vec3 Normal;    // Interpolated normal in world space
in vec2 TexCoords;

uniform sampler2D texture_diffuse1; // Or wallTexture, floorTexture etc.

// Material properties (can be uniforms if they vary per object)
// For simplicity, we'll hardcode some or use texture as baseColor.
// uniform vec3 objectColor; // Could be used if not texturing
uniform float material_shininess = 32.0f;
uniform float material_specularStrength = 0.5f; // How strong the specular highlight is

// Light properties (passed as uniforms)
uniform vec3 light_direction; // For directional light (direction TO the light source)
uniform vec3 light_color;
uniform float light_ambientIntensity = 0.2f;
// uniform vec3 light_position; // For point lights later
// uniform vec3 light_diffuse;  // Can combine with light_color
// uniform vec3 light_specular; // Can combine with light_color

uniform vec3 viewPos; // Camera position in world space

void main() {
    vec3 norm = normalize(Normal); // Ensure normal is normalized
    vec3 lightDir = normalize(-light_direction); // For directional, lightDir is constant
                                                // Use normalize(light_position - FragPos) for point light

    // Ambient
    vec3 ambient = light_ambientIntensity * light_color;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0); // Lambertian factor
    vec3 diffuse = diff * light_color;

    // Specular (Phong)
    vec3 viewDir = normalize(viewPos - FragPos); // Vector from fragment to camera
    vec3 reflectDir = reflect(-lightDir, norm);  // Reflection of lightDir around normal
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material_shininess);
    vec3 specular = material_specularStrength * spec * light_color; // Specular light color

    // Combine results with texture color
    vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 result = (ambient + diffuse) * textureColor + specular; // Add specular highlights separately
    
    FragColor = vec4(result, 1.0);
    // FragColor = vec4(norm * 0.5 + 0.5, 1.0); // Useful for debugging normals
}
