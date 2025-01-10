#ifndef LEVEL3D_H
#define LEVEL3D_H

#include <memory>

#include "entity3D.h"
#include "player3D.h"

class Level3D : public Entity3D {
private:
    std::shared_ptr<Model> model;
public:
    Level3D(Vector3 pos, std::string modelPath);
    ~Level3D();

    virtual void deInit() override;

    void setModelShader(Shader& shader);

    std::shared_ptr<Model> getModelSharedPtr();

    virtual void update() override;
    virtual void render() override;
};

#endif