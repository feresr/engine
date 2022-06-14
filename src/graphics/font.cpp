#include "font.h"
#include "Content.h"
#include "TexturePacker.h"
#include <SDL.h>
#include "Log.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

namespace Engine
{

    Font::Font(const std::string &path, int pixel_height = 8)
    {
        // Find and read .ttf file
        auto assets = Content::path().append(path);
        auto ptr = SDL_RWFromFile(assets.c_str(), "rb"); // rb = read + binary
        auto size = SDL_RWsize(ptr);
        std::vector<uint8_t> ttf_buffer;
        ttf_buffer.reserve(size);
        SDL_RWread(ptr, ttf_buffer.data(), sizeof(unsigned char), size);
        SDL_RWclose(ptr);

        std::shared_ptr<stbtt_fontinfo> info{new stbtt_fontinfo()};
        if (stbtt_InitFont(info.get(), ttf_buffer.data(), 0) == 0)
        {
            ENGINE_CORE_ERROR("Unable to parse font file");
        }

        buffer = std::move(ttf_buffer);
        font = info;

        // Get font info
        stbtt_fontinfo *fontInfo = (stbtt_fontinfo *)font.get();
        float scale = stbtt_ScaleForMappingEmToPixels(fontInfo, pixel_height);
        stbtt_GetFontVMetrics(fontInfo, &ascent, &descent, &lineGap);
        ascent *= scale;
        descent *= scale;

        // Pack font into bitmap
        auto packer = Engine::TexturePacker();
        for (int ch = 32; ch < 128; ch++)
        {
            Character character;
            int g = stbtt_FindGlyphIndex(fontInfo, ch);
            int x0, y0, x1, y1;
            stbtt_GetGlyphBitmapBox(fontInfo, g, scale, scale, &x0, &y0, &x1, &y1);
            character.offset_y = y0;
            int gw = x1 - x0;
            int gh = y1 - y0;
            int pw = gw, ph = gh;
            int ps = gw * gh;

            // allocate in the heap, the packer will own (and delete) this data
            Engine::Color *pixels = new Engine::Color[ps];
            stbtt_MakeGlyphBitmap(fontInfo, (uint8_t *)pixels, pw, ph, pw, scale, scale, g);

            int len = gw * gh;
            for (int a = (len - 1) * sizeof(Engine::Color), b = (len - 1); b >= 0; a -= sizeof(Engine::Color), b -= 1)
            {
                auto p = ((uint8_t *)pixels)[b];
                ((uint8_t *)pixels)[a] = p;
                ((uint8_t *)pixels)[a + 1] = p;
                ((uint8_t *)pixels)[a + 2] = p;
                ((uint8_t *)pixels)[a + 3] = p;
            }
            packer.addEntry(ch, gw, gh, pixels);
            characters[ch] = character;
        }

        texture = packer.pack();
        for (int ch = 32; ch < 128; ch++)
        {
            auto rect = packer.getEntryRect(ch);
            Character &character = characters[ch];
            int advance, offsetX;
            stbtt_GetCodepointHMetrics(fontInfo, ch, &advance, &offsetX);
            character.advance = advance * scale;
            character.offset_x = offsetX * scale;
            character.texture = Engine::Subtexture(texture, *rect);
            characters[ch] = character;
        }
    }

    const Engine::Character &Font::getCharacter(const char *text) const
    {
        return characters.at(*text);
    }

    int Engine::Font::getAdvance(int ch1, int ch2) const
    {
        int advance = characters.at(ch1).advance;
        int kern = stbtt_GetCodepointKernAdvance(static_cast<stbtt_fontinfo *>(font.get()), ch1, ch2);
        return advance + kern;
    }

    int Engine::Font::getWidth(const char* text) const {
        auto txt = text;
        float width = 0;
        while (*txt) {
            auto character = getCharacter(txt);
            width += character.offset_x; // this character left padding
            width += getAdvance(*txt, *(txt + 1)); // next character space
            ++txt;
        }
        return width;
    }
}
