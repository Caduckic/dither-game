#ifndef STATES_H
#define STATES_H

#include "raylib.h"

#include "../include/rlights.h"
#include "../entities/level3D.h"
#include "../entities/player3D.h"

enum StateType {
    NO_STATE = 0,
    LOGO_STATE,
    MENU_STATE,
    PLAY_STATE
};

class State {
protected:
    StateType currentState = NO_STATE;
    StateType nextState = NO_STATE;
    bool stateRender = true;
    bool reverseInit = false;
    bool popState = false;
    bool reloadState = false;

    void PopSelf();
public:
    State();
    ~State();

    virtual void init();
    virtual void deInit();
    virtual void update(Camera2D& camera2D, Camera3D& camera3D) = 0;
    virtual void render(const Camera2D& camera2D, const Camera3D& camera3D) = 0;
    
    StateType getCurrentState() const;

    StateType getNextState() const;

    bool shouldStateRender() const;

    bool isReverseInit() const;

    bool shouldStatePop() const;

    bool shouldStateReload() const;
};

class LogoState : public State {
private:
    int framesCounter;

    float logoPositionX;
    float logoPositionY;

    int lettersCount;

    float topSideRecWidth;
    float leftSideRecHeight;

    float bottomSideRecWidth;
    float rightSideRecHeight;

    int state = 0;
    float alpha = 1.0f;
public:
    LogoState();

    ~LogoState();

    virtual void update(Camera2D& camera2D, Camera3D& camera3D) override;

    virtual void render(const Camera2D& camera2D, const Camera3D& camera3D) override;
};

class MenuState : public State {
private:
    Vector2 logoPosition;
    Vector2 logoSize;

    Vector2 buttonsLeftTop;

    Vector2 buttonSize;
    float buttonSpacing;
    
    int buttonCount;

    Font font;
    int fontSize;

    bool quitGame;
public:
    MenuState();
    ~MenuState();

    virtual void init() override;

    virtual void deInit() override;

    virtual void update(Camera2D& camera2D, Camera3D& camera3D) override;

    virtual void render(const Camera2D& camera2D, const Camera3D& camera3D) override;
};

class PlayState : public State {
private:
    int renderDiv;
    RenderTexture2D frameBuffer;
    int lastResWidth;
    int lastResHeight;

    Shader ditherShader;
    float ditherThres;
    int ditherThresLoc;
    int ditherRenderResLoc;
    int ditherPaletteLoc;
    bool ditherOn;

    Level3D level3D;
    Shader levelShader;
    Light lights[MAX_LIGHTS];

    Player3D player3D;

    RenderTexture2D crtFrameBuffer;
    Model crtScreenModel;
    Vector2 rectPos;

    Shader imageFlipShader;
    Shader grayMaskShader;
public:
    PlayState();
    ~PlayState();

    virtual void init() override;

    virtual void deInit() override;

    virtual void update(Camera2D& camera2D, Camera3D& camera3D) override;

    virtual void render(const Camera2D& camera2D, const Camera3D& camera3D) override;
};

#endif