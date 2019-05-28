#pragma once

#ifndef CRYSTAL_H
#define CRYSTAL_H

#include "GameObject.h"

class Crystal : public GameObject
{

public:

    void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>,  std::shared_ptr<Program>, glm::vec3 view, glm::vec3);

    Crystal(glm::vec3 min, glm::vec3 max, int num, objType type, ColorCollectGameplay * ccg);
    virtual ~Crystal() {};

private:


};

#endif