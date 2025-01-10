#include <iostream>

#include "player3D.h"
#include "raymath.h"

void Player3D::setPosition(Vector3 pos) {
    position = pos;
    camera.position = Vector3Add(position, {0.0f, size.y, 0.0f});
    camera.target = Vector3Add(camera.position, lastCamForward);
}

// loops for every mesh we check angles around the player for walls
void Player3D::applyModelXZCollisionPosition(const std::shared_ptr<Model> model, float playerHeightOffset, unsigned int resolution, unsigned int meshIndex) {
    for (int j {0}; j < resolution; j++) {
        Vector3 rayDir = Vector3RotateByAxisAngle({0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, (45.0f*j)*DEG2RAD);
        // above function is broken so I need to clamp the values if they go over an abs of 1
        if (abs(rayDir.x) > 1.1f) rayDir.x = 0.0f;
        if (abs(rayDir.z) > 1.1f) rayDir.z = 0.0f;

        Ray ray;
        ray.direction = rayDir;
        ray.position = Vector3Add(position, {0.0f, playerHeightOffset, 0.0f});

        RayCollision rayCol = GetRayCollisionMesh(ray, model->meshes[meshIndex], model->transform);
        float threshold = 0.3f;
        float halfWidth = size.x/2;
        if (rayCol.hit && rayCol.distance < halfWidth && abs(Vector3Angle(rayCol.point, ray.position)) < threshold) {
            Vector3 reverseRayDir = Vector3RotateByAxisAngle(rayDir, {0.0f, 1.0f, 0.0f}, 180*DEG2RAD);
            Vector3 newPosition = {
                rayCol.point.x + (reverseRayDir.x * halfWidth),
                position.y,
                rayCol.point.z + (reverseRayDir.z * halfWidth)
            };
            setPosition(newPosition);
        }
    }
}

// for every mesh we check if there is a floor the floor ray hits
void Player3D::applyModelYCollisionPosition(const std::shared_ptr<Model> model, unsigned int meshIndex) {
    Ray floorRay;
    floorRay.direction = {0.0f, -1.0f, 0.0f};
    floorRay.position = Vector3Add(position, {0.0f, groundCheckHeight, 0.0f});

    RayCollision floorRayCol = GetRayCollisionMesh(floorRay, model->meshes[meshIndex], model->transform);

    if (floorRayCol.hit && floorRayCol.distance < groundCheckHeight) {
        setPosition({position.x, floorRayCol.point.y, position.z});
        velY = 0.0f;
        // playerGrounded = true;
    }
    else {
        // playerGrounded = false;
    }
}

void Player3D::applyModelCollisionPosition(const std::shared_ptr<Model> model) {
    for (unsigned int i {0}; i < model->meshCount; i ++) {
        applyModelXZCollisionPosition(model, groundCheckHeight, 16, i);
        applyModelXZCollisionPosition(model, size.y/2, 16, i);

        applyModelYCollisionPosition(model, i);
    }
}

void Player3D::applyModelCollisions(const std::vector<std::shared_ptr<Model>> models) {
    for (auto model : models) {
        applyModelCollisionPosition(model);
    }
}

const Camera3D& Player3D::getCamera() {
    return camera;
}

Player3D::Player3D() {
    init();
}

Player3D::~Player3D() = default;

void Player3D::init() {
    camera.fovy = 45.0f;
    // camera.position = Vector3{0.0f, 0.0f, 0.0f};
    camera.projection = CAMERA_PERSPECTIVE;
    camera.target = Vector3{0.0f, 0.0f, -1.0f};
    camera.up = Vector3{0.0f, 1.0f, 0.0f};
    yaw = 0.0f;
    pitch = 0.0f;

    setPosition({0.0f, 0.0f, 0.0f});

    lastCamForward = Vector3Subtract(camera.target, camera.position);
    size = {0.5f, 1.0f, 0.5f};
    speed = 3.0f;
    mouseSensitivity = 0.1f;

    groundCheckHeight = 0.2f;
    velY = 0.0f;
}

void Player3D::deInit() {

}

void Player3D::update() {
    // walking inputs
    Vector2 moveDir = {0.0f, 0.0f};
    if (IsKeyDown(KEY_W))
        moveDir.y += 1.0f;
    if (IsKeyDown(KEY_S))
        moveDir.y -= 1.0f;
    if (IsKeyDown(KEY_A))
        moveDir.x += 1.0f;
    if (IsKeyDown(KEY_D))
        moveDir.x -= 1.0f;
    
    lastCamForward = Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    lastCamForward = Vector3Normalize({lastCamForward.x, 0.0f, lastCamForward.z});

    Vector3 right = Vector3Normalize(Vector3CrossProduct(camera.up, lastCamForward));

    Vector3 movement = {0.0f, 0.0f, 0.0f};

    // front and back
    movement = Vector3Add(movement, Vector3Scale(lastCamForward, moveDir.y));

    // left and right
    movement = Vector3Add(movement, Vector3Scale(right, moveDir.x));

    // apply movement
    setPosition(Vector3Add(position, Vector3Scale(movement, speed * GetFrameTime())));

    // update mouse look
    if (IsCursorOnScreen()) {
        Vector2 mouseDelta = GetMouseDelta();

        yaw -= mouseDelta.x * mouseSensitivity;
        pitch -= mouseDelta.y * mouseSensitivity;

        // limit the pitch to avoid flipping
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        // calc new target
        Vector3 newTarget = {0.0f, 0.0f, 0.0f};

        newTarget.x = sinf(DEG2RAD * yaw) * cosf(DEG2RAD * pitch);
        newTarget.y = sinf(DEG2RAD * pitch);
        newTarget.z = cosf(DEG2RAD * yaw) * cosf(DEG2RAD * pitch);

        // set the target
        camera.target = Vector3Add(camera.position, newTarget);
    }

    // jump
    if (IsKeyPressed(KEY_SPACE))
        velY = 3.0f;

    std::cout << "velY: " << velY << ", PlayerPos: x " << position.x << ", y" << position.y << ", z" << position.z << '\n';
    // apply gravity
    float gravity = 9.8f;
    velY -= gravity * GetFrameTime();

    position.y += velY * GetFrameTime();
    camera.position.y += velY * GetFrameTime();
    camera.target.y += velY * GetFrameTime();

    // sets the last forward so the collisions later will correctly set the camera target
    lastCamForward = Vector3Subtract(camera.target, camera.position);
}

void Player3D::render() {
    
}