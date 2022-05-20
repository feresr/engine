#pragma once
#include "Engine.h"

class Background : public Engine::Component
{
    public:
    
    Background(int width, int height);

    bool awake() override;

    void update() override;

    void render(Engine::Batch &batch) override;

    private:
    int width, height;
    Engine::Sprite* sky = Content::findSprite("background");
};