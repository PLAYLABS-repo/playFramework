#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Image.h"

bool Image::load(const char* path)
{
    int n;
    unsigned char* data = stbi_load(path, &w, &h, &n, 4);
    if (!data) return false;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return true;
}
