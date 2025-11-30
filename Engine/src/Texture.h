#pragma once

#include <glad/glad.h>
#include <string>

class Texture
{
public:
    Texture();
    ~Texture();

    // Initialize DevIL (called automatically)
    static void InitDevIL();

    void CreateCheckerboard();

    // Load texture from file
    bool LoadFromLibraryOrFile(const std::string& path, bool flipVertically = true);

    // Bind/Unbind
    void Bind();
    void Unbind();

    GLuint GetID() const { return textureID; }

    // Texture information
    int GetWidth() const { return width; }
    int GetHeight() const { return height; }
    int GetChannels() const { return nrChannels; }

private:
    GLuint textureID;
    int width;
    int height;
    int nrChannels;
};