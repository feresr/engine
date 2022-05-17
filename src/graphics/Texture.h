#pragma once

#include <memory>
#include <glad/glad.h>
#include "Sampler.h"

namespace Engine {

    enum class TextureFormat {
        None,
        R,
        RG,
        RGB,
        RGBA,
        DepthStencil,
        Count
    };


    class Texture {

    private:
        GLuint id;
        int width;
        int height;
        TextureSampler sampler;
        TextureFormat format;
        GLenum GLInternalFormat;
        GLenum GLFormat;
        GLenum GLType;


    protected:
        Texture() = default;

        Texture(int width, int height, TextureFormat format);
    public:
        bool framebufferParent;

        Texture(const Texture&) = delete;

        Texture(Texture&&) = delete;

        Texture& operator=(const Texture&) = delete;

        Texture& operator=(Texture&&) = delete;

        ~Texture();

        static std::shared_ptr<Texture> create(int width, int height, unsigned char* rgba);

        static std::shared_ptr<Texture> create(int width, int height, TextureFormat format);

        static std::shared_ptr<Texture> create(const char* file);

        GLuint getId();

        [[nodiscard]] int getWidth() const;

        [[nodiscard]] int getHeight() const;

        [[nodiscard]] TextureFormat getFormat() const;

        // Sets the data of the Texture.
        // Note that the pixel buffer should be in the same format as the Texture. There is no row padding.
        // If the pixel buffer isn't the same size as the texture, it will set the minimum available amount of data.
        void set_data(unsigned char* data) const;

        // Gets the data of the Texture.
        // Note that the pixel buffer will be written to in the same format as the Texture,
        // and you should allocate enough space for the full texture. There is no row padding.
        virtual void get_data(unsigned char* data);

        // Returns true if the Texture is part of a FrameBuffer
        [[nodiscard]] bool isFramebuffer() const;

        void updateSampler(const TextureSampler& newSampler);
    };
}
