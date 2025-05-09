#pragma once

#include <glad/glad.h>
#include <string>

class Texture {
public:
    // Texture ID
    unsigned int ID;

    // Texture dimensions
    int width;
    int height;
    int channels;

    // Constructor (generates texture from image file)
    Texture(const std::string& path, bool generateMipmaps = true);

    // Destructor
    ~Texture();

    // Bind texture to a texture unit
    void Bind(unsigned int textureUnit = 0) const;

    // Unbind texture
    void Unbind() const;

    // Static utility functions
    static void SetTextureWrapMode(unsigned int textureID, GLenum wrapS, GLenum wrapT);
    static void SetTextureFilterMode(unsigned int textureID, GLenum minFilter, GLenum magFilter);
};