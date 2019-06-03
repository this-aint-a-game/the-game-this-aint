//
// Created by Caroline Cullen on 2019-06-02.
//

#ifndef MOON_H
#define MOON_H

#include "GameObject.h"
class Moon : public GameObject{
public:

    void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>,  std::shared_ptr<Program>, glm::vec3 view, glm::vec3, ColorCollectGameplay* ccg);
    void setPosition(float, float);
    Moon(glm::vec3 min, glm::vec3 max, int num, objType type, ColorCollectGameplay * ccg);
    virtual ~Moon() {};
};


#endif //THE_GAME_THIS_AINT_MOON_H
