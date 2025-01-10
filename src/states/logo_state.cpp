#include "states.h"

// pretty much a copy of the intro in ray's template but in a class and with a camera for a fixed scale and position

LogoState::LogoState() {
    framesCounter = 0;
    lettersCount = 0;

    logoPositionX = (float)GetScreenWidth()/2 - 128;
    logoPositionY = (float)GetScreenHeight()/2 - 128;

    topSideRecWidth = 16.0f;
    leftSideRecHeight = 16.0f;
    bottomSideRecWidth = 16.0f;
    rightSideRecHeight = 16.0f;

    state = 0;
    alpha = 1.0f;
}

LogoState::~LogoState() = default;

void LogoState::update(Camera2D& camera2D, Camera3D& camera3D) {
    if (IsWindowResized()) {
        float width = GetScreenWidth();
        float height = GetScreenHeight();

        camera2D.offset = Vector2{(float)width/2, (float)height/2};
        camera2D.target = Vector2{(float)width/2, (float)height/2};

        if (width > height) {
            camera2D.zoom = (float)height/500;
        }
        else {
            camera2D.zoom = (float)width/500;
        }
    }

    logoPositionX = (float)GetScreenWidth()/2 - 128;
    logoPositionY = (float)GetScreenHeight()/2 - 128;
    if (state == 0)                 // State 0: Top-left square corner blink logic
    {
        framesCounter++;

        if (framesCounter == 80)
        {
            state = 1;
            framesCounter = 0;      // Reset counter... will be used later...
        }
    }
    else if (state == 1)            // State 1: Bars animation logic: top and left
    {
        topSideRecWidth += 8.0f;
        leftSideRecHeight += 8.0f;

        if (topSideRecWidth >= 256.0f) {
            topSideRecWidth = 256.0f;
            state = 2;
        }
    }
    else if (state == 2)            // State 2: Bars animation logic: bottom and right
    {
        bottomSideRecWidth += 8.0f;
        rightSideRecHeight += 8.0f;

        if (bottomSideRecWidth >= 256.0f) {
            bottomSideRecWidth = 256.0f;
            state = 3;
        }
    }
    else if (state == 3)            // State 3: "raylib" text-write animation logic
    {
        framesCounter++;

        if (lettersCount < 10)
        {
            if (framesCounter/12)   // Every 12 frames, one more letter!
            {
                lettersCount++;
                framesCounter = 0;
            }
        }
        else    // When all letters have appeared, just fade out everything
        {
            if (framesCounter > 200)
            {
                alpha -= 0.02f;

                if (alpha <= 0.0f)
                {
                    alpha = 0.0f;
                    nextState = MENU_STATE;   // Jump to next screen
                }
            }
        }
    }
}

void LogoState::render(const Camera2D& camera2D, const Camera3D& camera3D) {
    ClearBackground(RAYWHITE);

    BeginMode2D(camera2D);
        if (state == 0)         // Draw blinking top-left square corner
        {
            if ((framesCounter/10)%2) DrawRectangleV({logoPositionX, logoPositionY}, {16.0f, 16.0f}, BLACK);
        }
        else if (state == 1)    // Draw bars animation: top and left
        {
            DrawRectangleV({logoPositionX, logoPositionY}, {topSideRecWidth, 16.0f}, BLACK);
            DrawRectangleV({logoPositionX, logoPositionY}, {16.0f, leftSideRecHeight}, BLACK);
        }
        else if (state == 2)    // Draw bars animation: bottom and right
        {
            DrawRectangleV({logoPositionX, logoPositionY}, {topSideRecWidth, 16.0f}, BLACK);
            DrawRectangleV({logoPositionX, logoPositionY}, {16, leftSideRecHeight}, BLACK);

            DrawRectangleV({logoPositionX + 240.0f, logoPositionY}, {16.0f, rightSideRecHeight}, BLACK);
            DrawRectangleV({logoPositionX, logoPositionY + 240.0f}, {bottomSideRecWidth, 16.0f}, BLACK);
        }
        else if (state == 3)    // Draw "raylib" text-write animation + "powered by"
        {
            DrawRectangleV({logoPositionX, logoPositionY}, {topSideRecWidth, 16.0f}, Fade(BLACK, alpha));
            DrawRectangleV({logoPositionX, logoPositionY + 16.0f}, {16.0f, leftSideRecHeight - 32.0f}, Fade(BLACK, alpha));

            DrawRectangleV({logoPositionX + 240.0f, logoPositionY + 16.0f}, {16.0f, rightSideRecHeight - 32.0f}, Fade(BLACK, alpha));
            DrawRectangleV({logoPositionX, logoPositionY + 240.0f}, {bottomSideRecWidth, 16.0f}, Fade(BLACK, alpha));

            DrawRectangleV({(float)GetScreenWidth()/2.0f - 112.0f, (float)GetScreenHeight()/2.0f - 112.0f}, {224.0f, 224.0f}, Fade(RAYWHITE, alpha));

            DrawText(TextSubtext("raylib", 0, lettersCount), GetScreenWidth()/2 - 44, GetScreenHeight()/2 + 48, 50, Fade(BLACK, alpha));

            if (framesCounter > 20) DrawText("powered by", logoPositionX, logoPositionY - 27, 20, Fade(DARKGRAY, alpha));
        }
    EndMode2D();
}
