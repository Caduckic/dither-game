#ifndef PLAYER3D_H
#define PLAYER3D_H

#include <vector>
#include <memory>

#include "entity3D.h"

class Player3D : public Entity3D {
private:
    Camera3D camera;
    Vector3 lastCamForward;
    float yaw;
    float pitch;

    Vector3 size;
    float speed;
    float mouseSensitivity;

    float groundCheckHeight;
    float velY;

    void applyModelXZCollisionPosition(const std::shared_ptr<Model> model, float playerHeightOffset, unsigned int resolution, unsigned int meshIndex);
    void applyModelYCollisionPosition(const std::shared_ptr<Model> model, unsigned int meshIndex);
    void applyModelCollisionPosition(const std::shared_ptr<Model> model);
public:
    Player3D();
    ~Player3D();

    void applyModelCollisions(const std::vector<std::shared_ptr<Model>> models);

    virtual void setPosition(Vector3 pos) override;

    const Camera3D& getCamera();

    virtual void init() override;
    virtual void deInit() override;

    virtual void update() override;
    virtual void render() override;
};

#endif