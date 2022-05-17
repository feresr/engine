//
// Created by Fernando Raviola on 10/15/21.
//

#include "TexturePacker.h"

void Engine::TexturePacker::addEntry(int id, int w, int h, Engine::Color *color)
{
    entries.emplace_back(id, w, h, color);
}

void set_pixels(Engine::Color *dst, const Engine::RectI &rect, Engine::Color *data, int w)
{
    for (int y = 0; y < rect.h; y++)
    {
        int to = rect.x + (rect.y + y) * w;
        int from = (y * rect.w);
        memcpy(dst + to, data + from, sizeof(Engine::Color) * rect.w);
    }
}

std::shared_ptr<Engine::Texture> Engine::TexturePacker::pack()
{

    // Todo: just list all textures horizontally for now
    //  doesn't even consider full alhpa?
    // Find the tallest image
    int maxHeight = 0;
    int widthSum = 0;
    for (auto &entry : entries)
    {
        if (entry.h > maxHeight)
            maxHeight = entry.h;
        widthSum += entry.w;
    }

    Engine::Color data[widthSum * maxHeight];
    int x = 0;
    for (auto &entry : entries)
    {
        entry.rect = Engine::Rect(x, 0, entry.w, entry.h);
        set_pixels(data, Engine::RectI(x, 0, entry.w, entry.h), entry.color, widthSum);
        x += entry.w;
    }

    return Engine::Texture::create(widthSum, maxHeight, (unsigned char *)data);
}

Engine::Rect *Engine::TexturePacker::getEntryRect(int id)
{
    for (auto &entry : entries)
    {
        if (entry.id == id)
            return &entry.rect;
    }
    return nullptr;
}

void Engine::TexturePacker::clear()
{
    entries.clear();
}

Engine::TexturePacker::Entry::~Entry()
{
    if (color)
    {
        delete[] color;
        color = nullptr;
    }
}
