#include <iostream>

#include "state_manager.h"

StateManager::StateManager() {};
StateManager::~StateManager() = default;

// loops thru all states calling deInit to unload all files
void StateManager::deInitialize() {
    popStates(states.size() - 1);
    // last manual pop as the functions prevent you from popping all states
    states.back()->deInit();
    states.pop_back();
}

void StateManager::pushState(std::shared_ptr<State> newState) {
    states.push_back(newState);
}

void StateManager::popState() {
    // checks to ensure at least 1 state is always active
    if (states.size() > 1) {
        states.back()->deInit();
        states.pop_back();
        // checks new current state is set to be reinitialized
        if (states.back()->isReverseInit())
            states.back()->init();
    }
    else
        std::cout << "ERROR! Cannot remove final state\n";
}

void StateManager::popStates(const unsigned int pops) {
    // checks to ensure at least 1 state is always active
    if (pops >= states.size()) {
        std::cout << pops << "ERROR! Cannot pop all or more than loaded states\n";
    }
    else {
        for(size_t i {0}; i < pops; i++) {
            states.back()->deInit();
            states.pop_back();
            // checks new current state is set to be reinitialized
            if (states.back()->isReverseInit() && i == pops - 1)
                states.back()->init();
        }
    }
}

void StateManager::update(Camera2D& camera2D, Camera3D& camera3D) {
    // only updates back state
    states.back()->update(camera2D, camera3D);

    bool popCurrentState = states.back()->shouldStatePop();

    // checks for next state being set and adds it to the "stack"
    if (states.back()->getNextState() != NO_STATE && !popCurrentState) {
        StateType nextState = states.back()->getNextState();
        switch (nextState)
        {
        case LOGO_STATE:
            pushState(std::make_shared<LogoState>());
            break;
        case MENU_STATE:
            pushState(std::make_shared<MenuState>());
            break;
        case PLAY_STATE:
            pushState(std::make_shared<PlayState>());
            break;
        default:
            break;
        }
    }

    if (popCurrentState)
        popState();
}

void StateManager::render(const Camera2D& camera2D, const Camera3D& camera3D) const {
    // loops over every state to render all of them
    for (auto iter = states.begin(); iter != states.end(); ++iter) {
        // special case if state is flagged not to render when below another
        if (iter->get()->shouldStateRender()) {
            iter->get()->render(camera2D, camera3D);
        }
    }
}
