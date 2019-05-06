#pragma once

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#define GROUND_SIZE 75 // TODO change to terrain size

#include <string>
#include <vector>
#include "BoundingBox.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "ColorCollectGameplay.h"

#include <glm/gtc/type_ptr.hpp>

class GameObject
{

public:
    enum objType{strawberry, crystal1, crystal2, crystal3, player};

    // TODO take num and type out of this call
    virtual void initObject(glm::vec3 min, glm::vec3 max, int num, objType type, ColorCollectGameplay * ccg) = 0;
    virtual void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>, std::shared_ptr<Program>, glm::vec3 view) = 0;

    glm::vec3 currentPos;
    glm::vec3 scale;

    int color;
    objType type;
    std::shared_ptr<Program> prog;
    ColorCollectGameplay* ccg;

    bool isCollided(glm::vec3 v)
    {
        return bb->isCollided(v, currentPos, this->scale);
    }

    bool isCollided(BoundingBox *box)
    {
        return bb->isCollided(box, currentPos, this->scale);
    }

    virtual BoundingBox* getBB()
    {
        return bb->get(currentPos);
    }

    // TODO should this be in ColorCollectGameplay?
    void SetMaterial(int i, Program *prog)
    {
        switch (i)
        {
            case 0: // GAME RED
                glUniform3f(prog->getUniform("MatAmb"), 0.666, 0.262, 0.258);
                glUniform3f(prog->getUniform("MatDif"), 0.766, 0.362, 0.358);
                glUniform3f(prog->getUniform("MatSpec"), 0.866, 0.462, 0.458);
                glUniform1f(prog->getUniform("shine"), 47.0f);
                break;

            case 1: // GAME ORANGE
                glUniform3f(prog->getUniform("MatAmb"), 0.833, 0.482, 0.003);
                glUniform3f(prog->getUniform("MatDif"), 0.933, 0.582, 0.103);
                glUniform3f(prog->getUniform("MatSpec"), 0.933, 0.682, 0.403);
                glUniform1f(prog->getUniform("shine"), 47.0f);
                break;

            case 2: // GAME YELLOW
                glUniform3f(prog->getUniform("MatAmb"), 0.560, 0.509, 0.239);
                glUniform3f(prog->getUniform("MatDif"), 0.610, 0.559, 0.289);
                glUniform3f(prog->getUniform("MatSpec"), 0.760, 0.709, 0.439);
                glUniform1f(prog->getUniform("shine"), 47.0f);
                break;

            case 3: // GAME GREEN
                glUniform3f(prog->getUniform("MatAmb"),   0.425, 0.545, 0.117);
                glUniform3f(prog->getUniform("MatDif"), 0.425, 0.545, 0.117);
                glUniform3f(prog->getUniform("MatSpec"), 0.725, 0.745, 0.317);
                glUniform1f(prog->getUniform("shine"), 47.0f);
                break;

            case 4: // GAME BLUE

                /*
                glUniform3f(prog->getUniform("MatAmb"), 0.049, 0.576, 0.617);
                glUniform3f(prog->getUniform("MatDif"), 0.049, 0.576, 0.617);
                glUniform3f(prog->getUniform("MatSpec"), 0.049, 0.676, 0.717);
                */

                glUniform3f(prog->getUniform("MatAmb"), 0.08, 0.67, 0.8);
                glUniform3f(prog->getUniform("MatDif"), 0.08, 0.67, 0.8);
                glUniform3f(prog->getUniform("MatSpec"), 0.18, 0.77, 0.9);
                glUniform1f(prog->getUniform("shine"), 47.0f);

                break;


            case 5: // GAME VIOLET
                glUniform3f(prog->getUniform("MatAmb"), 0.11f, 0.00f, 0.25f);
                glUniform3f(prog->getUniform("MatDif"), 0.11f, 0.00f, 0.25f);
                glUniform3f(prog->getUniform("MatSpec"), 0.21f, 0.1f, 0.35f);
                glUniform1f(prog->getUniform("shine"), 47.f);

                //glUniform3f(prog->getUniform("MatAmb"), 0.445f, 0.194f, 0.488f);
                //glUniform3f(prog->getUniform("MatDif"), 0.445f, 0.194f, 0.488f);
                //glUniform3f(prog->getUniform("MatSpec"), 0.545f, 0.294f, 0.588f);
                //glUniform1f(prog->getUniform("shine"), 47.0f);

                break;

        }
    }

    float getRand(float min, float max)
    {
        float r = rand() / (float) RAND_MAX;
        return (1.0f - r) * min + r * max;
    }


    GameObject() { };
    virtual ~GameObject() {};

protected:
    BoundingBox *bb;

};

#endif