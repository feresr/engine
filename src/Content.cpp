#include "Content.h"
#include "Aseprite.h"
#include "Sprite.h"
#include "Log.h"
#include <dirent.h>
#include "Application.h"
#include <json/json.h>

#include "TexturePacker.h"
#include "fstream"
// for convenience
using json = nlohmann::json;

std::map<std::string, Engine::Sprite> Content::sprites{};
std::vector<MapInfo> Content::maps{};

std::pair<std::string, Engine::Sprite> loadSprite(const std::string &assets, const std::string &name)
{
    Engine::Aseprite aseprite(assets + name);

    std::string n{name.substr(0, name.size() - 4)};
    auto sprite = Engine::Sprite(name.substr(0, name.size() - 4));

    if (!aseprite.slices.empty())
        sprite.pivot = {aseprite.slices[0].pivotX, aseprite.slices[0].pivotY};

    auto packer = Engine::TexturePacker();

    for (int i = 0; i < aseprite.frames.size(); i++)
    {
        auto image = aseprite.frames[i].image;
        packer.addEntry(i, image.width, image.height, image.pixels);
    }

    auto texture = packer.pack();

    // If there are no tags, just get the first frame...
    if (aseprite.tags.empty()) {
        Engine::Animation &anim = sprite.addAnimation();
        anim.name = n;
        auto &frame = anim.frames.emplace_back();
        frame.texture = Engine::Subtexture(texture, *packer.getEntryRect(0));
    }

    // else 
    for (auto &tag : aseprite.tags)
    {
        Engine::Animation &anim = sprite.addAnimation();
        anim.name = tag.name;

        for (int frameIndex = tag.from; frameIndex <= tag.to; frameIndex++)
        {
            auto &frame = anim.frames.emplace_back();

            auto &image = aseprite.frames[frameIndex].image;
            frame.durationMillis = aseprite.frames[frameIndex].duration;

            anim.duration += frame.durationMillis;

            frame.texture = Engine::Subtexture(
                // TODO: should pack textures into one
                // Engine::Texture::create(image.width, image.height, (unsigned char *) image.pixels),
                texture,
                // TODO check this, they should not be hardcoded
                *packer.getEntryRect(frameIndex));
        }
    }
    return std::pair{n, std::move(sprite)};
}
// Returns the path to the /assets/ folder
std::string Content::path() {
    std::filesystem::path app { Engine::Application::path() };
    app /= "assets";
    auto directory = opendir(app.c_str());
    int depth = 0;
    while(!directory && depth < 20) {
        app = app.parent_path().parent_path();
        app /= "assets";
        directory = opendir(app.c_str());
        depth++;
    }
    if (!directory) {
        ENGINE_CORE_ERROR("Could not find 'assets' directory...");
        return nullptr;
    }
    closedir(directory);
    auto assetsPath = std::string { app.c_str() };
    return assetsPath.append("/");
}

void Content::load()
{
    sprites.clear();
    maps.clear();
    auto assets = path();
    ENGINE_INFO(assets.c_str());
    auto directory = opendir(assets.c_str());
    dirent *dir = readdir(directory);
    while (dir != nullptr)
    {
        std::string name{dir->d_name};

        // Load sprites
        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".ase") == 0)
        {
            auto sprite = loadSprite(assets, name);
            sprites.insert(sprite);
        }

        if (name.size() > 4 && name.compare(name.size() - 4, 4, ".png") == 0)
        {
            std::string n{name.substr(0, name.size() - 4)};
            auto sprite = Engine::Sprite(name.substr(0, name.size() - 4));

            auto &anim = sprite.addAnimation();
            anim.duration = 0;

            auto &frame = anim.frames.emplace_back();
            frame.durationMillis = 0;
            auto tex = Engine::Texture::create((assets + name).c_str());
            frame.texture = Engine::Subtexture(
                // TODO: should pack textures into one
                // Engine::Texture::create(image.width, image.height, (unsigned char *) image.pixels),
                tex,
                // TODO check this, they should not be hardcoded
                Engine::Rect(0, 0, tex->getWidth(), tex->getHeight()));

            sprites.insert({n, sprite});
        }

        // Load worlds (map info)
        if (name.size() > 4 && name.compare(name.size() - 6, 6, ".world") == 0)
        {
            std::ifstream reader(assets + name, std::ios::binary);
            auto world = json::parse(reader);
            auto j = world["maps"];

            for (auto &it : j)
            {
                MapInfo &mp = maps.emplace_back();
                mp.fileName = assets + it.at("fileName").get<std::string>();
                mp.rect = Engine::RectI(
                    it.at("x").get<int>(),
                    it.at("y").get<int>(),
                    it.at("width").get<int>(),
                    it.at("height").get<int>());
            }
        }

        dir = readdir(directory);
    }

    closedir(directory);
}

std::vector<MapInfo> Content::getMaps() {
    return maps;
}
Engine::Sprite *Content::findSprite(const std::string &name)
{
    return &sprites.at(name);
}

MapInfo *Content::findMapInfo(const glm::ivec2 &position)
{
    for (auto &map : maps)
    {
        if (map.rect.contains(position))
            return &map;
    }
    return nullptr;
}
