#include "Application.h"
#include "Log.h"

/*
void* operator new(size_t size) {
    std::cout << "Allocating " << size <<  "bytes\n";
    return malloc(size);
}

void operator delete(void* memory, std::size_t size) noexcept {
    std::cout << "Freeing " << size <<  "bytes\n";
    free(memory);
}
*/

int main(int argc, char *argv[]) {
    Engine::Log::init();
    ENGINE_CORE_INFO("Launching app...");
    Engine::Application *application = Engine::createApplication();
    application->run();
    delete application;
    return 0;
}
