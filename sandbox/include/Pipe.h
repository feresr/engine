#pragma once
#include "Engine.h"

class Pipe : public Engine::Component
{
public:
    Pipe(bool top = false);

    bool awake() override;

    void update() override;

    void render(Engine::Batch &batch) override;

    bool top = false;
private:
};