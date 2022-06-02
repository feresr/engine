#pragma once

#include <string>
#include "Component.h"
#include "Color.h"

namespace Engine {

    class Sprite;
    class Animation;

    class SpriteComponent : public Component {

    public:
        SpriteComponent() = delete;

        glm::vec2 scale = {1.0f, 1.0f};
        float rotation = 0.0f;

        explicit SpriteComponent(const std::string &sprite);

        void play(const std::string &animation);

        void render(Batch &batch) override;

        void update() override;


        int getCurrentAnimDuration();

        void setColor(Engine::Color color);

        // todo: getCurrentAnimSize doesn't work becaue the packer packs alpha values
        glm::ivec2 getCurrentAnimSize();

        bool awake() override;

    private:
        int frameIndex{0};
        float frameCounter{0};

        std::string spriteName;
        std::string animationName;

        Engine::Sprite* getSprite();
        Engine::Animation* getAnimation();

        Engine::Color color = 0xffffff;
    };
}