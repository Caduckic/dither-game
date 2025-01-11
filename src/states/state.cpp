#include "states.h"

void State::PopSelf() {
    popState = true;
}

State::State() = default;
State::~State() = default;

void State::init() {};
void State::deInit() {};

StateType State::getCurrentState() const {
    return currentState;
}

StateType State::getNextState() const {
    return nextState;
}

bool State::shouldStateRender() const {
    return stateRender;
}

bool State::isReverseInit() const {
    return reverseInit;
}

bool State::shouldStatePop() const {
    return popState;
}

bool State::shouldStateReload() const {
    return reloadState;
}