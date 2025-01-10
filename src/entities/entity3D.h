#ifndef ENTITY3D_H
#define ENTITY3D_H

#include "raylib.h"
#include <string>

class Entity3D {
protected:
    Vector3 position;
public:
    Entity3D();
    ~Entity3D();

    virtual void init();
    virtual void deInit();

    virtual void setPosition(Vector3 pos);
    virtual Vector3 getPosition() const;

    virtual void update() = 0;
    virtual void render() = 0;
};

#endif