#include "level3D.h"
#include "raymath.h"

Level3D::Level3D(Vector3 pos, std::string modelPath) {
    position = pos;
    model = std::make_shared<Model>(LoadModel(modelPath.c_str()));
}

Level3D::~Level3D() = default;

void Level3D::deInit() {
    UnloadModel(*model);
}

void Level3D::setModelShader(Shader& shader) {
    for (int i {0}; i < model->materialCount; i++) {
        model->materials[i].shader = shader;
    }
}

std::shared_ptr<Model> Level3D::getModelSharedPtr() {
    return model;
}

void Level3D::update() {

}

void Level3D::render() {
    DrawModelEx(*model, position, {0.0f, 0.0f, 0.0f}, 0.0f, {1.0f, 1.0f, 1.0f}, WHITE);
}