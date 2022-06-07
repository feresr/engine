#pragma once

#include "iostream"
#include <SDL.h>
#include "Ecs.h"

int main(int argc, char* argv[]);

namespace Engine {

    constexpr int FPS = 60;
    constexpr double FRAME_DURATION = 1000.0f / FPS;

    class Application {
    public:
        explicit Application(std::string name = "untitled game",
                             int width = 1280, int height = 720,
                             bool fullScreen = false);

        virtual ~Application();

        static Application& get() { return *instance; }

        bool isRunning = false;

        // Gets the working path

        static const char* path();

        SDL_Window* window{};


    protected:

        virtual void update() = 0;

        virtual void render() = 0;

        virtual void handleEvent(SDL_Event& event) = 0;

        const std::string appName;
    private:

        void run();

        SDL_Event event{};

        static Application* instance;
        
        SDL_GLContext context;

        friend int::main(int argc, char* argv[]);

    };

    // To be defined by client
    extern Application* createApplication();
}

