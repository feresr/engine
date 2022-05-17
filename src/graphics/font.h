#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include "Texture.h"
#include "Subtexture.h"

namespace Engine
{
    class TexturePacker;

    struct Character
    {
        Engine::Subtexture texture;
        float advance;
        float offset_x;
        float offset_y;
    };

    class Font
    {
    public:
        Font() = delete;
        Font(const std::string &path, int pixel_height);

        const Engine::Character &getCharacter(const char *text) const;
        int getAdvance(int ch1, int ch2) const;

        int ascent, descent, lineGap;
    private:
        std::shared_ptr<void> font;
        std::vector<uint8_t> buffer;
        std::shared_ptr<Engine::Texture> texture;
        std::map<char, Character> characters;
    };
} // namespace Engine
