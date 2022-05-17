#pragma once

namespace Engine {

    enum class TextureFilter {
        None,
        Linear,
        Nearest
    };

    enum class TextureWrap {
        None,
        Clamp,
        Repeat
    };

    struct TextureSampler {

        TextureFilter filter;
        TextureWrap wrapX;
        TextureWrap wrapY;

        TextureSampler() :
                filter(TextureFilter::Linear),
                wrapX(TextureWrap::Repeat),
                wrapY(TextureWrap::Repeat) {}

        TextureSampler(TextureFilter filter) :
                filter(filter),
                wrapX(TextureWrap::Repeat),
                wrapY(TextureWrap::Repeat) {}

        TextureSampler(TextureFilter filter, TextureWrap wrap_x, TextureWrap wrap_y) :
                filter(filter),
                wrapX(wrap_x),
                wrapY(wrap_y) {}

        bool operator==(const TextureSampler& rhs) const {
            return filter == rhs.filter &&
                    wrapX == rhs.wrapX &&
                    wrapY == rhs.wrapY;
        }

        bool operator!=(const TextureSampler& rhs) const {
            return !(*this == rhs);
        }
    };
}
