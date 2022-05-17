#include "Engine.h"

class SandboxApplication : public Engine::Application {

    public:
    Engine::Batch batch;

    SandboxApplication() : Engine::Application("Flappy Bird", 800, 600, false){}

    void update() override {
    }

    void render() override {
        auto time = Engine::Time::getTime() / 1000.0f;
        static auto color = Engine::Color(0xff000000);
        color.r = sinf(time * .2) * 255;
        color.g = sinf(time * .4) * 255;
        color.b = sinf(time * .3) * 255;
        Engine::FrameBuffer::BackBuffer()->clear(color);
    }

    void handleEvent(SDL_Event &event) override {

    }
};

Engine::Application *Engine::createApplication()
{
    return new SandboxApplication();
}