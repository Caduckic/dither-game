#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>

#include "states.h"
#include "raymath.h"

#define RLIGHTS_IMPLEMENTATION
#include "../include/rlights.h"

PlayState::PlayState() : level3D {{0.0f, 0.0f, 0.0f}, "resources/models/test-level/basic-test-level.obj"} {
    init();
}

PlayState::~PlayState() = default;

void PlayState::init() {
    currentState = PLAY_STATE;
    // loading and setting up the frameBuffer render texture
    renderDiv = 4;
    lastResWidth = GetScreenWidth()/renderDiv;
    lastResHeight = GetScreenHeight()/renderDiv;
    frameBuffer = LoadRenderTexture(lastResWidth, lastResHeight);

    // loading and setting up the framebuffer dither shader
#if defined(PLATFORM_WEB)
    ditherShader = LoadShader("resources/shaders/lightingWeb.vs", "resources/shaders/ditherWeb.fs");
#else
    ditherShader = LoadShader("resources/shaders/lighting.vs", "resources/shaders/dither.fs");
#endif
    ditherThres = 0.5f;
    ditherThresLoc = GetShaderLocation(ditherShader, "threshold");
    SetShaderValue(ditherShader, ditherThresLoc, &ditherThres, SHADER_UNIFORM_FLOAT);

    // Wacks8 palette
    // Vector3 palette[8] = {
    //     {0.039, 0.055, 0.2},    // DARK Blue
    //     {0.784, 0.137, 0.165},  // Red
    //     {0.945, 0.878, 0.22},   // Yellow
    //     {0.078, 0.588, 0.298},  // Green
    //     {0.549, 0.71, 0.929},   // Cyan
    //     {0.169, 0.396, 0.886},  // Blue
    //     {0.773, 0.137, 0.675},  // Purple
    //     {0.894, 0.859, 0.941}     // LIGHT Blue
    // };

    Vector3 palette[8] = {
        {0.0f, 0.0f, 0.0f},         // Black
        {1.0f, 1.0f, 1.0f},         // White
        {0.843f, 0.149f, 0.22f},    // Red
        {0.957f, 0.376f, 0.212f},   // Orange
        {1.0f, 0.824f, 0.247f},     // Yellow
        {0.149f, 0.329f, 0.486f},   // Dark Cyan
        {0.129f, 0.631f, 0.475f},   // Green
        {0.0f, 0.988f, 0.953f}      // Light Blue
    };


    for (int i {0}; i < sizeof(palette) / sizeof(Vector2); i++) {
        int ditherPaletteILoc = GetShaderLocation(ditherShader, TextFormat("palette[%01i]", i));
        SetShaderValue(ditherShader, ditherPaletteILoc, &palette[i], SHADER_UNIFORM_VEC3);
    }

    Vector2 renderRes = {(float)GetScreenWidth()/renderDiv, (float)GetScreenHeight()/renderDiv};
    ditherRenderResLoc = GetShaderLocation(ditherShader, "renderRes");
    SetShaderValue(ditherShader, ditherRenderResLoc, &renderRes, SHADER_UNIFORM_VEC2);

    ditherOn = true;

#if defined(PLATFORM_WEB)
    levelShader = LoadShader("resources/shaders/lightingWeb.vs", "resources/shaders/lightingWeb.fs");
#else
    levelShader = LoadShader("resources/shaders/lighting.vs", "resources/shaders/lighting.fs");
#endif
    level3D.setModelShader(levelShader);

    levelShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(levelShader, "viewPos");
    int ambientLoc = GetShaderLocation(levelShader, "ambient");
    float ambient[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    SetShaderValue(levelShader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);

    // setting up the model shaders lights
    lights[0] = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, -2 }, (Vector3){0,0,0}, WHITE, 0.7f, levelShader);
    lights[1] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, 2 }, (Vector3){0,0,0}, WHITE, 0.7f, levelShader);
    lights[2] = CreateLight(LIGHT_POINT, (Vector3){ -2, 1, 2 }, (Vector3){0,0,0}, WHITE, 0.7f, levelShader);
    lights[3] = CreateLight(LIGHT_POINT, (Vector3){ 2, 1, -2 }, (Vector3){0,0,0}, WHITE, 0.7f, levelShader);

    crtFrameBuffer = LoadRenderTexture(520, 380);
    crtScreenModel = LoadModel("resources/models/CRT-Screen-Test.obj");

    crtScreenModel.materials[0].maps[0].texture = crtFrameBuffer.texture;

    rectPos = {20.0f, 300.0f};

    #if defined(PLATFORM_WEB)
        imageFlipShader = LoadShader("resources/shaders/lightingWeb.vs", "resources/shaders/flipWeb.fs");
        grayMaskShader = LoadShader("resources/shaders/lightingWeb.vs", "resources/shaders/grayMaskWeb.fs");
    #else
        imageFlipShader = LoadShader("resources/shaders/lighting.vs", "resources/shaders/flip.fs");
        grayMaskShader = LoadShader("resources/shaders/lighting.vs", "resources/shaders/grayMask.fs");
    #endif
    std::cout << player3D.getCamera().target.x << '\n';
}

// unloads the frameBuffer, shaders and the model
void PlayState::deInit() {
    UnloadRenderTexture(frameBuffer);
    UnloadShader(ditherShader);

    level3D.deInit();
    player3D.deInit();
    UnloadShader(levelShader);
    resetLightsCount();
    
    UnloadRenderTexture(crtFrameBuffer);
    UnloadModel(crtScreenModel);
}

void PlayState::update(Camera2D& camera2D, Camera3D& camera3D) {
    // basic first-person mouse disabling
    if (IsKeyPressed(KEY_M)) EnableCursor();
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) DisableCursor();
    
    // pops state after update is finished and returns us to menustate
    if (IsKeyPressed(KEY_Q)) {
        PopSelf();
        EnableCursor();
    }

    if (IsKeyPressed(KEY_X))
        ditherOn = !ditherOn;

    level3D.update();
    player3D.update();

    player3D.applyModelCollisions({level3D.getModelSharedPtr()});

    const Camera3D playerCam = player3D.getCamera();
    float cameraPos[3] = { playerCam.position.x, playerCam.position.y, playerCam.position.z };
    SetShaderValue(levelShader, levelShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

    if (IsKeyDown(KEY_L)) {
        ditherThres += GetFrameTime() * 5.f;
        std::cout << ditherThres << '\n';
    }
    if (IsKeyDown(KEY_K)) {
        ditherThres -= GetFrameTime() * 5.f;
    }

    SetShaderValue(ditherShader, ditherThresLoc, &ditherThres, SHADER_UNIFORM_FLOAT);

    // checks for window size changes and resets the framebuffer to match
    int currentResWidth = GetScreenWidth()/renderDiv;
    int currentResHeight = GetScreenHeight()/renderDiv;

    if (currentResWidth != lastResWidth || currentResHeight != lastResHeight) {
        lastResWidth = currentResWidth;
        lastResHeight = currentResHeight;

        Vector2 renderRes = {(float)currentResWidth, (float)currentResHeight};
        SetShaderValue(ditherShader, ditherRenderResLoc, &renderRes, SHADER_UNIFORM_VEC2);

        // reloads the frame buffer with the update resolution
        frameBuffer = LoadRenderTexture(currentResWidth, currentResHeight);
    }

    // update rect pos
    Vector2 rectDir = {0.0f, 0.0f};
    if (IsKeyDown(KEY_UP))
        rectDir.y -= 1.0f;
    if (IsKeyDown(KEY_DOWN))
        rectDir.y += 1.0f;
    if (IsKeyDown(KEY_LEFT))
        rectDir.x -= 1.0f;
    if (IsKeyDown(KEY_RIGHT))
        rectDir.x += 1.0f;
    
    rectPos.x += rectDir.x * 200 * GetFrameTime();
    rectPos.y += rectDir.y * 200 * GetFrameTime();
}

void PlayState::render(const Camera2D& camera2D, const Camera3D& camera3D) {
    // draw the backpack lights and grid to the framebuffer
    level3D.setModelShader(levelShader);
    BeginTextureMode(frameBuffer);
        ClearBackground(BLACK);

        BeginMode3D(player3D.getCamera());

            level3D.render();
            DrawGrid(40, 1.0f);

            for (auto light: lights) {
                DrawSphere(light.position, 0.2f, light.color);
            }

        EndMode3D();
    EndTextureMode();

    // draw the frame buffer as a rectangle texture over the screen
    if (ditherOn) BeginShaderMode(ditherShader);
        ClearBackground(BLACK);
        DrawTextureEx(frameBuffer.texture, {0.0f, 0.0f}, 0.0f, renderDiv, WHITE);
    if (ditherOn) EndShaderMode();

    // draw the CRT screen
    BeginTextureMode(crtFrameBuffer);
        ClearBackground(YELLOW);
        // DrawRectangle(30, 200, 300, 50, GREEN);
        DrawRectangleV(rectPos, {300.0f, 50.0f}, GREEN);
    EndTextureMode();

    // render the CRT screen into the 3D enviroment
    level3D.setModelShader(grayMaskShader);
    BeginTextureMode(frameBuffer);
        Color col = {0, 0, 0, 0};
        ClearBackground(col);
        BeginMode3D(player3D.getCamera());
            DrawModel(crtScreenModel, {0.0f, 0.0f, 1.0f}, 0.3f, WHITE);
            level3D.render();
            // DrawBillboard(camera3D, paletteImage, {0.0f, 0.0f, 20.0f}, 1.0f, WHITE);
        EndMode3D();
    EndTextureMode();

    BeginShaderMode(imageFlipShader);
        DrawTextureEx(frameBuffer.texture, {0.0f, 0.0f}, 0.0f, renderDiv, WHITE);
    EndShaderMode();

    DrawFPS(20, 20);
}
