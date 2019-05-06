#pragma once

#ifndef CRYSTAL_H
#define CRYSTAL_H

#include "GameObject.h"

class Crystal : public GameObject
{

public:

    void initObject(glm::vec3, glm::vec3, int, objType, ColorCollectGameplay * ccg);
    void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>,  std::shared_ptr<Program>, glm::vec3 view);

    Crystal();
    virtual ~Crystal() {};

private:


};

#endif