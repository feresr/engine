#pragma once

#include <vector>
#include <rect.h>
#include "Texture.h"
#include "Color.h"
#include "rectI.h"

namespace Engine
{
    // TODO: Drop this in favour of https://github.com/nothings/stb/blob/master/stb_rect_pack.h
    class TexturePacker
    {

    private:
        struct Entry
        {
            Entry(int id, int w, int h, Engine::Color *color) : id{id}, w{w}, h{h}, color{color} {}

            // copy constructor
            Entry(const Entry &other)
            {
                this->rect = other.rect;
                this->id = other.id;
                this->w = other.w;
                this->h = other.h;
                this->color = new Color[w * h];
                memcpy(this->color, other.color, sizeof(Color) * h * w);
            };
            Entry &operator=(const Entry &other) = delete;

            // move constructor
            Entry(Entry &&other)
            {
                this->rect = other.rect;
                this->id = other.id;
                this->w = other.w;
                this->h = other.h;
                this->color = other.color;
                other.color = nullptr;
            }
            Entry &operator=(Entry &&other) = delete;

            int id, w, h;
            Engine::Color *color;
            Engine::Rect rect;

            ~Entry();
        };

        std::vector<Entry> entries;

    public:
        Engine::Rect *getEntryRect(int id);

        void addEntry(int id, int w, int h, Engine::Color *color);

        std::shared_ptr<Engine::Texture> pack();

        void clear();
    };

}
