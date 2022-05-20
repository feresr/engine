#include "Engine.h"

class Slider : public Engine::Component
{

public:
    bool awake() override;

    void update() override;

    void render(Engine::Batch &batch) override;

    int velocity = 1;

};