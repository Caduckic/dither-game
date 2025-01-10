/*******************************************************************************************
*
*   raylib game template
*
*   <Game title>
*   <Game description>
*
*   This game has been created using raylib (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2021 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <memory>

#include "game.h"
#include "exiter.h"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

Exiter Exiter::instance;

//----------------------------------------------------------------------------------
// Local Variables Definition (local to this module)
//----------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 800;

//----------------------------------------------------------------------------------
// Local Functions Declaration
//----------------------------------------------------------------------------------
static void UpdateDrawFrame(std::shared_ptr<Game> game);          // Update and draw one frame
static void UpdateDrawFrame(void* game);          // Update and draw one frame

//----------------------------------------------------------------------------------
// Main entry point
//----------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //---------------------------------------------------------
    std::shared_ptr<Game> game = std::make_shared<Game>(screenWidth, screenHeight);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop_arg(UpdateDrawFrame, game.get(), 60, 1);
#else
    // game->setGameFPS(60);            // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose() && !Exiter::Get().ShouldCloseGame())    // Detect window close button or ESC key
    {
        UpdateDrawFrame(game);
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    game->deInitialize();
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Update and draw game frame
static void UpdateDrawFrame(std::shared_ptr<Game> game)
{
    // Update
    game->update();
    // Draw
    game->render();
}

static void UpdateDrawFrame(void* gamePtr)
{
    Game* game = static_cast<Game*>(gamePtr);
    // Update
    game->update();
    // Draw
    game->render();
}