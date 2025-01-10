#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include <memory>
#include <vector>

#include "states.h"

class StateManager {
private:
    std::vector<std::shared_ptr<State>> states;
public:
    StateManager();
    ~StateManager();

    void deInitialize();

    void pushState(std::shared_ptr<State> newState);

    void popState();

    void popStates(const unsigned int pops);

    void update(Camera2D& camera2D, Camera3D& camera3D);

    void render(const Camera2D& camera2D, const Camera3D& camera3D) const;
};

#endif