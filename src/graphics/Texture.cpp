//
// Created by Fernando Raviola on 20/02/2021.
//

#include "Texture.h"
#include "Log.h"

#define STB_IMAGE_IMPLEMENTATION

#include "stb/stb_image.h"

namespace Engine
{

    Texture::Texture(int width, int height, TextureFormat format)
    {
        id = 0;
        this->width = width;
        this->height = height;
        sampler = TextureSampler(TextureFilter::None, TextureWrap::None, TextureWrap::None);
        this->format = format;
        framebufferParent = false;
        GLInternalFormat = GL_RED;
        GLFormat = GL_RED;
        GLType = GL_UNSIGNED_BYTE;

        int maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        if (width > maxTextureSize || height > maxTextureSize)
        {
            ENGINE_CORE_ERROR("Exceeded Max Texture Size of {}", maxTextureSize);
            return;
        }

        // This tells opengl how to pack a row of pixels (pixels are contiguos, rows doen't have to be)
        // ex: GL_UNPACK_ALIGNMENT of 4 will only allow for rows that are with of multiples of 4 (will add empty padding)
        // so if you upload 2 pixels in format RGB (GL_UNSIGNED_BYTE) RGBRGB, this is not a multiple of 4, opengl will ad 2 bytes extra
        // and look for the next row at the wrong place.
        // todo: maybe pack textures in multiples of 2? and leave this to the default 4.
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        switch (format)
        {
        case TextureFormat::R:
        {
            GLInternalFormat = GL_RED;
            GLFormat = GL_RED;
            GLType = GL_UNSIGNED_BYTE;
            break;
        }
        case TextureFormat::RG:
        {
            GLInternalFormat = GL_RG;
            GLFormat = GL_RG;
            GLType = GL_UNSIGNED_BYTE;
            break;
        }
        case TextureFormat::RGB:
        {
            GLInternalFormat = GL_RGB8;
            GLFormat = GL_RGB;
            GLType = GL_UNSIGNED_BYTE;
            break;
        }
        case TextureFormat::RGBA:
        {
            GLInternalFormat = GL_RGBA;
            GLFormat = GL_RGBA;
            GLType = GL_UNSIGNED_BYTE;
            break;
        }
        case TextureFormat::DepthStencil:
        {
            GLInternalFormat = GL_DEPTH24_STENCIL8;
            GLFormat = GL_DEPTH_STENCIL;
            GLType = GL_UNSIGNED_INT_24_8;
            break;
        }
        default:
        {
            ENGINE_CORE_ERROR("Invalid Texture Format {}", format);
            return;
        }
        }

        glGenTextures(1, &id);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GLInternalFormat, width, height, 0, GLFormat, GLType, nullptr);
    }

    std::shared_ptr<Texture> Engine::Texture::create(int width, int height, unsigned char *rgba)
    {
        auto texture = create(width, height, TextureFormat::RGBA);
        if (texture)
            texture->set_data(rgba);
        return texture;
    }

    std::shared_ptr<Texture> Engine::Texture::create(int width, int height, Engine::TextureFormat format)
    {
        ENGINE_ASSERT(width > 0 && height > 0, "Texture with and height must be greater than 0");
        ENGINE_ASSERT((int)format > (int)TextureFormat::None && (int)format < (int)TextureFormat::Count,
                      "Invalid Texture Format");
        auto *texture = new Texture(width, height, format);
        return std::shared_ptr<Engine::Texture>(texture);
    }

    std::shared_ptr<Texture> Engine::Texture::create(const char *file)
    {
        int width, height, channels;

        unsigned char *img = stbi_load(file, &width, &height, &channels, 4);
        if (!img)
            ENGINE_CORE_ERROR("Could not load texture {}", file);
        auto *tex = new Texture(width, height, (TextureFormat)channels);
        tex->set_data(img);
        stbi_image_free(img);
        return std::shared_ptr<Texture>(tex);
    }

    Texture::~Texture()
    {
        if (id > 0)
            glDeleteTextures(1, &id);
    }

    int Texture::getWidth() const
    {
        return width;
    }

    int Texture::getHeight() const
    {
        return height;
    }

    TextureFormat Texture::getFormat() const
    {
        return format;
    }

    void Texture::set_data(unsigned char *data) const
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GLInternalFormat, width, height, 0, GLFormat, GLType, data);
    }

    void Texture::get_data(unsigned char *data)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, id);
        glGetTexImage(GL_TEXTURE_2D, 0, GLInternalFormat, GLType, data);
    }

    bool Texture::isFramebuffer() const
    {
        return framebufferParent;
    }

    void Texture::updateSampler(const TextureSampler &newSampler)
    {
        if (sampler != newSampler)
        {
            sampler = newSampler;
            glBindTexture(GL_TEXTURE_2D, id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            sampler.filter == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                            sampler.filter == TextureFilter::Nearest ? GL_NEAREST : GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                            sampler.wrapX == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                            sampler.wrapY == TextureWrap::Clamp ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        }
    }

    GLuint Texture::getId()
    {
        return id;
    }

}
