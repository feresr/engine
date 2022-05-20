#pragma once
#include "Engine.h"
#include <functional>

class Bird : public Engine::Component
{
    public:
    
    Bird();

    bool awake() override;

    void update() override;

    void render(Engine::Batch &batch) override;

    std::function<void(void)> onPipeCrash;


    bool dead = false;
};