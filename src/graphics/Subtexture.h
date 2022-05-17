#pragma once

#include "Texture.h"
#include "rect.h"

namespace Engine {

    // A sprite is a view into a texture.
    // todo: make subtexture extend texture?
    class Subtexture {

    public:

        Subtexture();

        Subtexture(const std::shared_ptr<Texture>& texture, Rect source);

        std::shared_ptr<Texture> texture;

        // non-normalised texture coordinates
        Rect rect{};

        [[nodiscard]] float width() const { return rect.w; }

        [[nodiscard]] float height() const { return rect.h; }

    };

}