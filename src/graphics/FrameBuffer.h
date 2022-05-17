#pragma once

#include <memory>
#include <vector>
#include "Texture.h"
#include "glad/glad.h"
#include "Color.h"


namespace Engine {

    typedef std::vector<std::shared_ptr<Texture>> Attachments;

    class FrameBuffer {

    private:
        GLuint id;
        int mWidth;
        int mHeight;
        Attachments mAttachments;

    protected:
        FrameBuffer() = default;
        FrameBuffer(int width, int height, const TextureFormat* attachments, int attachmentCount, GLuint id);

    public:
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer(FrameBuffer&&) = delete;
        FrameBuffer& operator=(const FrameBuffer&) = delete;
        FrameBuffer& operator=(FrameBuffer&&) = delete;

        virtual ~FrameBuffer();

        /**
         * Creates a FrameBuffer with a single color attachment,
         * if it fails, it'll return an invalid FrameBuffer reference
         */
        static std::shared_ptr<FrameBuffer> create(int width, int height);

        /**
         * Creates a FrameBuffer with the given texture attachments. You must provide at least one.
         * if it fails, it'll return an invalid FrameBuffer reference
         */
        static std::shared_ptr<FrameBuffer> create(int width, int height, const TextureFormat* attachments, int attachmentCount);

        static std::shared_ptr<FrameBuffer> BackBuffer();

        // Gets the list of Attachments from the FrameBuffer
        virtual Attachments& attachments();

        // Gets the list of Attachments from the FrameBuffer
        [[nodiscard]] virtual const Attachments& attachments() const;

        // Gets the Attachment at a given index from the FrameBuffer
        [[nodiscard]] virtual const std::shared_ptr<Texture>& attachment(int index) const;

        // Gets the width of the FrameBuffer
        [[nodiscard]] virtual int width() const;

        // Gets the height of the FrameBuffer
        [[nodiscard]] virtual int height() const;

        void bind() const;

        // Clears the FrameBuffer
        void clear() const;
        void clear(Color color) const;
    };
}

