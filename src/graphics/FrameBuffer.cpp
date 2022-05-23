#include "FrameBuffer.h"
#include "Log.h"
#include <glad/glad.h>
#include <Application.h>

// 4 color attachments + 1 depth/stencil
#define MAX_ATTACHMENTS 5

namespace Engine
{
    class BackBuffer final : public FrameBuffer
    {

    public:
        Attachments &attachments() override
        {
            ENGINE_ASSERT(false, "Back buffer doesn't have any attachments");
            return FrameBuffer::attachments();
        }

        [[nodiscard]] const Attachments &attachments() const override
        {
            ENGINE_ASSERT(false, "Back buffer doesn't have any attachments");
            return FrameBuffer::attachments();
        }

        [[nodiscard]] const std::shared_ptr<Texture> &attachment(int index) const override
        {
            ENGINE_ASSERT(false, "Back buffer doesn't have any attachments");
            return FrameBuffer::attachment(index);
        }

        [[nodiscard]] int width() const override
        {
            int width, height;
            SDL_GL_GetDrawableSize(Application::get().window, &width, &height);
            return width;
        }

        [[nodiscard]] int height() const override
        {
            int width, height;
            SDL_GL_GetDrawableSize(Application::get().window, &width, &height);
            return height;
        }
    };
}

Engine::FrameBuffer::FrameBuffer(int width, int height, const Engine::TextureFormat *textureFormats, int attachmentCount,
                                 GLuint id) : id{id}
{

    this->mWidth = width;
    this->mHeight = height;

    glBindFramebuffer(GL_FRAMEBUFFER, id);

    for (int i = 0; i < attachmentCount; i++)
    {
        auto tex = Texture::create(width, height, textureFormats[i]);
        tex->framebufferParent = true;
        mAttachments.push_back(tex);

        if (textureFormats[i] != TextureFormat::DepthStencil)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, tex->getId(), 0);
        }
        else
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, tex->getId(), 0);
        }
    }
}

std::shared_ptr<Engine::FrameBuffer> Engine::FrameBuffer::create(int width, int height)
{
    static const TextureFormat format = TextureFormat::RGBA;
    return create(width, height, &format, 1);
}

std::shared_ptr<Engine::FrameBuffer>
Engine::FrameBuffer::BackBuffer()
{
    static auto frameBuffer = std::shared_ptr<Engine::BackBuffer>{new class Engine::BackBuffer()};
    return frameBuffer;
}

std::shared_ptr<Engine::FrameBuffer>
Engine::FrameBuffer::create(int width, int height, const TextureFormat *textureFormat, int textureCount)
{

    ENGINE_ASSERT(width > 0 && height > 0, "FrameBuffer width and height must be larger than 0");
    ENGINE_ASSERT(textureCount <= MAX_ATTACHMENTS, "Exceeded maximum textureFormat count");
    ENGINE_ASSERT(textureCount > 0, "At least one textureFormat must be provided");

    int colorCount = 0;
    int depthCount = 0;

    for (int i = 0; i < textureCount; i++)
    {
        ENGINE_ASSERT(
            (int)textureFormat[i] > (int)TextureFormat::None && (int)textureFormat[i] < (int)TextureFormat::Count,
            "Invalid texture format");
        if (textureFormat[i] == TextureFormat::DepthStencil)
            depthCount++;
        else
            colorCount++;
    }

    ENGINE_ASSERT(depthCount <= 1, "FrameBuffer can only have 1 Depth/Stencil Texture");
    ENGINE_ASSERT(colorCount <= MAX_ATTACHMENTS - 1, "Exceeded maximum Color textureFormat count");

    GLuint id;
    glGenFramebuffers(1, &id);

    auto *frameBuffer = new Engine::FrameBuffer(width, height, textureFormat, textureCount, id);
    return std::shared_ptr<Engine::FrameBuffer>{frameBuffer};
}

Engine::FrameBuffer::~FrameBuffer()
{
    if (id > 0)
    {
        glDeleteFramebuffers(1, &id);
        id = 0;
    }
}

Engine::Attachments &Engine::FrameBuffer::attachments()
{
    return mAttachments;
}

const Engine::Attachments &Engine::FrameBuffer::attachments() const
{
    return mAttachments;
}

const std::shared_ptr<Engine::Texture> &Engine::FrameBuffer::attachment(int index) const
{
    return mAttachments[index];
}

int Engine::FrameBuffer::width() const
{
    return mWidth;
}

int Engine::FrameBuffer::height() const
{
    return mHeight;
}

void Engine::FrameBuffer::clear() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Engine::FrameBuffer::clear(Engine::Color color) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glDisable(GL_SCISSOR_TEST);
    glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Engine::FrameBuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}
