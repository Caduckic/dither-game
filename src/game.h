#ifndef GAME_H
#define GAME_H

#include "raylib.h"

#include "states/state_manager.h"

class Game {
private:
    StateManager stateManager;
    Camera2D camera2D;
    Camera3D camera3D;
public:
    Game(const int screenWidth, const int screenHeight);

    ~Game();

    void deInitialize();

    void setGameFPS(const int fps);

    void update();

    void render() const;
};

#endif