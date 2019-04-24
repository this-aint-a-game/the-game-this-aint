#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <vector>
#include <glm.hpp>
#include "BoundingBox.h"

class GameObject
{

public:


    bool collected;
    int hit_count = 0;
    virtual void initObject(glm::vec3, glm::vec3) = 0;
    virtual void update(float) = 0;
    virtual bool isCollided(glm::vec3) = 0;
    virtual bool isCollided(BoundingBox*) = 0;
    virtual BoundingBox* getBB() = 0;

    glm::vec3 currentPos;
    glm::vec3 velocity;

    GameObject() {};
    virtual ~GameObject() {};

protected:
    BoundingBox *bb;

};

#endif