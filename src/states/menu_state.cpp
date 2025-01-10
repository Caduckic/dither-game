#include "raygui.h"

#include "states.h"
#include "../exiter.h"

MenuState::MenuState() {
    init();
}

MenuState::~MenuState() = default;

void MenuState::init() {
    reverseInit = true;
    nextState = NO_STATE;
    stateRender = true;

    logoPosition = {(float)GetScreenWidth()/2 - 192, 64.0f};

    buttonsLeftTop = {(float)GetScreenWidth()/2 - 128, 224.0f};

    logoSize = {384.0f, 128.0f};

    buttonSize = {256.0f, 64.0f};
    buttonSpacing = 24.0f;

    buttonCount = 3;

    font = LoadFont("resources/images/mecha.png");
    fontSize = 48;

    quitGame = false;
}

// unloads relevant objects
void MenuState::deInit() {
    /*
        TODO I've had to turn font unloading off as it causes issues when closeWindow is called outside the game loop
        raylib seems to treat this.font like its the default font so when it trys to unload it to double unloads as this unloadfont call already does it?
        It's confusing
    */
    // UnloadFont(font);
}

void MenuState::update(Camera2D& camera2D, Camera3D& camera3D) {
    if (quitGame) {
        deInit();
        Exiter::Get().CloseGame();
        stateRender = false;
    }
    logoPosition = {(float)GetScreenWidth()/2 - 192, 64.0f};

    buttonsLeftTop = {(float)GetScreenWidth()/2 - 128, 224.0f};
}

void MenuState::render(const Camera2D& camera2D, const Camera3D& camera3D) {
    ClearBackground(RAYWHITE);

    GuiSetFont(font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    GuiLabel({logoPosition.x, logoPosition.y, logoSize.x, logoSize.y}, "Raylib Game Template");

    bool playGame = GuiButton({buttonsLeftTop.x, buttonsLeftTop.y, buttonSize.x, buttonSize.y}, "Play");

    bool openOptions = GuiButton({buttonsLeftTop.x, buttonsLeftTop.y + buttonSize.y + buttonSpacing, buttonSize.x, buttonSize.y}, "Options");

#if !defined(PLATFORM_WEB)
    quitGame = GuiButton({buttonsLeftTop.x, buttonsLeftTop.y + ((buttonSize.y + buttonSpacing) * 2), buttonSize.x, buttonSize.y}, "Quit");
#endif
    if (playGame) {
        nextState = PLAY_STATE;
        stateRender = false;
    }
    if (openOptions) {

    }
}
