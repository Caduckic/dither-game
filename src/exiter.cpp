#include "exiter.h"

// singleton class for game loop escape

Exiter::Exiter() : shouldClose{false} {};

Exiter& Exiter::Get() {
    return instance;
}

bool Exiter::ShouldCloseGame() {
    return shouldClose;
}

void Exiter::CloseGame() {
    shouldClose = true;
}

Exiter::~Exiter() = default;
