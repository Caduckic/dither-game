#define GRAPHICS_API_OPENGL_ES3
#include <iostream>

#include "game.h"

Game::Game(const int screenWidth, const int screenHeight) {
    InitWindow(screenWidth, screenHeight, "raylib game template");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    InitAudioDevice();

    stateManager = {};
    stateManager.pushState(std::make_shared<LogoState>());

    camera2D.offset = Vector2{(float)screenWidth/2, (float)screenHeight/2};
    camera2D.target = Vector2{(float)screenWidth/2, (float)screenHeight/2};
    camera2D.zoom = (float)screenHeight/500;
    camera2D.rotation = 0.0f;

    camera3D.fovy = 45.0f;
    camera3D.position = Vector3{0.0f, 0.0f, 0.0f};
    camera3D.projection = CAMERA_PERSPECTIVE;
    camera3D.target = Vector3{0.0f, 0.0f, -1.0f};
    camera3D.up = Vector3{0.0f, 1.0f, 0.0f};
}

Game::~Game() = default;


// unloads all loaded files and closes the window
void Game::deInitialize() {
    stateManager.deInitialize();
    CloseAudioDevice();
    CloseWindow();
}

void Game::setGameFPS(const int fps) {
    SetTargetFPS(fps);
}

void Game::update() {
    stateManager.update(camera2D, camera3D);
}

void Game::render() const {
    BeginDrawing();

        stateManager.render(camera2D, camera3D);

    EndDrawing();
}
