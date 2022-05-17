#pragma once
#include "Engine.h"

class Bird : public Engine::Component
{
    public:
    
    Bird();

    bool awake() override;

    void update() override;

    void render(Engine::Batch &batch) override;
};