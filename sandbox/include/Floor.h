#pragma once
#include "Engine.h"

class Floor : public Engine::Component
{
public:
    Floor(int width, int height);

    bool awake() override;

    void update() override;

    void render(Engine::Batch &batch) override;


private:
    Engine::Sprite *base = Content::findSprite("base");
    int width, height;
};