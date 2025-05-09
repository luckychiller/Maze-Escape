#include "Texture.h"
#include <iostream>

// Include stb_image.h for image loading (without implementation)
#include "../../external/stb_image.h"

Texture::Texture(const std::string& path, bool generateMipmaps)
    : ID(0), width(0), height(0), channels(0)
{
    // Generate texture
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    // Set default texture wrapping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image data
    stbi_set_flip_vertically_on_load(true); // Flip images vertically (OpenGL expects 0,0 at bottom left)
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (data)
    {
        GLenum format;
        if (channels == 1)
            format = GL_RED;
        else if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;
        else
        {
            std::cerr << "Unsupported number of channels: " << channels << std::endl;
            format = GL_RGB; // Default to RGB
        }

        // Create texture
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        // Generate mipmaps if requested
        if (generateMipmaps)
            glGenerateMipmap(GL_TEXTURE_2D);

        std::cout << "Texture loaded: " << path << " (" << width << "x" << height << ", " << channels << " channels)" << std::endl;
    }
    else
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }

    // Free image data
    stbi_image_free(data);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &ID);
}

void Texture::Bind(unsigned int textureUnit) const
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetTextureWrapMode(unsigned int textureID, GLenum wrapS, GLenum wrapT)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetTextureFilterMode(unsigned int textureID, GLenum minFilter, GLenum magFilter)
{
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glBindTexture(GL_TEXTURE_2D, 0);
}