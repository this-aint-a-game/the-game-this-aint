//
// Created by Caroline Cullen on 2019-05-27.
//

#ifndef PLANT_H
#define PLANT_H

#include "GameObject.h"

class Plant : public GameObject
{
public:

    void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>,  std::shared_ptr<Program>, glm::vec3 view, glm::vec3, ColorCollectGameplay* ccg);
    void setPosition(float, float);
    Plant(glm::vec3 min, glm::vec3 max, int num, objType type, ColorCollectGameplay * ccg);
    virtual ~Plant() {};
};


#endif //THE_GAME_THIS_AINT_PLANT_H
