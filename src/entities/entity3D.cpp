#include "entity3D.h"

Entity3D::Entity3D() = default;
Entity3D::~Entity3D() = default;

void Entity3D::init() {};
void Entity3D::deInit() {};

void Entity3D::setPosition(Vector3 pos) {
    position = pos;
}

Vector3 Entity3D::getPosition() const {
    return position;
}