#pragma once

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#define GROUND_SIZE 75 // TODO change to terrain size

#include <string>
#include <vector>
#include "BoundingSphere.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "ColorCollectGameplay.h"

#include <glm/gtc/type_ptr.hpp>

class GameObject
{

public:
    enum objType{strawberry, plant, moon};

    virtual void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>, std::shared_ptr<Program>, glm::vec3 view, glm::vec3 , ColorCollectGameplay* ccg) = 0;
    glm::vec3 currentPos;
    glm::vec3 scale;

    int color;
    objType type;
    std::shared_ptr<Program> prog;
    ColorCollectGameplay* ccg;
    int comparator_value = 0;


    bool isCollided(BoundingSphere *sphere)
    {
        return bs->isCollided(sphere, currentPos);
    }

    virtual BoundingSphere* getBS()
    {
        if(this->type == plant)
        {
            return bs->get(this->currentPos, 0.3f);
        }
        else
        {
            return bs->get(this->currentPos, 0.0f);
        }

    }

    // TODO should this be in ColorCollectGameplay?
    void SetMaterial(int i, Program *prog)
    {
        glm::vec3 a;
        glm::vec3 d;
        glm::vec3 s;

        switch (i)
        {
            case 0: // GAME RED
                a = ccg->redColor.ambient;
                d = ccg->redColor.diffuse;
                s = ccg->redColor.specular;

                glUniform3f(prog->getUniform("MatAmb"), a.x, a.y, a.z);
                glUniform3f(prog->getUniform("MatDif"), d.x, d.y, d.z);
                glUniform3f(prog->getUniform("MatSpec"), s.x, s.y, s.z);
                glUniform1f(prog->getUniform("shine"), 47.0f);

                break;

            case 1: // GAME ORANGE
                a = ccg->orangeColor.ambient;
                d = ccg->orangeColor.diffuse;
                s = ccg->orangeColor.specular;

                glUniform3f(prog->getUniform("MatAmb"), a.x, a.y, a.z);
                glUniform3f(prog->getUniform("MatDif"), d.x, d.y, d.z);
                glUniform3f(prog->getUniform("MatSpec"), s.x, s.y, s.z);
                glUniform1f(prog->getUniform("shine"), 47.0f);

                break;

            case 2: // GAME YELLOW
                a = ccg->yellowColor.ambient;
                d = ccg->yellowColor.diffuse;
                s = ccg->yellowColor.specular;

                glUniform3f(prog->getUniform("MatAmb"), a.x, a.y, a.z);
                glUniform3f(prog->getUniform("MatDif"), d.x, d.y, d.z);
                glUniform3f(prog->getUniform("MatSpec"), s.x, s.y, s.z);
                glUniform1f(prog->getUniform("shine"), 47.0f);

                break;

            case 3: // GAME GREEN

                a = ccg->greenColor.ambient;
                d = ccg->greenColor.diffuse;
                s = ccg->greenColor.specular;

                glUniform3f(prog->getUniform("MatAmb"), a.x, a.y, a.z);
                glUniform3f(prog->getUniform("MatDif"), d.x-0.2, d.y-0.2, d.z-0.2);
                glUniform3f(prog->getUniform("MatSpec"), s.x, s.y, s.z);
                glUniform1f(prog->getUniform("shine"), 47.0f);

                break;

            case 4: // GAME BLUE

                a = ccg->blueColor.ambient;
                d = ccg->blueColor.diffuse;
                s = ccg->blueColor.specular;

                glUniform3f(prog->getUniform("MatAmb"), a.x, a.y, a.z);
                glUniform3f(prog->getUniform("MatDif"), d.x, d.y, d.z);
                glUniform3f(prog->getUniform("MatSpec"), s.x, s.y, s.z);
                glUniform1f(prog->getUniform("shine"), 47.0f);

                break;


            case 5: // GAME VIOLET
                a = ccg->violetColor.ambient;
                d = ccg->violetColor.diffuse;
                s = ccg->violetColor.specular;

                glUniform3f(prog->getUniform("MatAmb"), a.x, a.y, a.z);
                glUniform3f(prog->getUniform("MatDif"), d.x, d.y, d.z);
                glUniform3f(prog->getUniform("MatSpec"), s.x, s.y, s.z);
                glUniform1f(prog->getUniform("shine"), 47.0f);

                break;

        }
    }

    float getRand(float min, float max)
    {
        float r = rand() / (float) RAND_MAX;
        return (1.0f - r) * min + r * max;
    }

    bool operator =(const GameObject& obj2) const
    {
        if((this->type == strawberry) && (obj2.type == strawberry) && ((this->color == obj2.color)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool operator <(const GameObject& obj2) const
    {
        if(this->comparator_value == 0)
        {
            return this->currentPos.x < obj2.currentPos.x;
        }
        else
        {
            return this->currentPos.z < obj2.currentPos.z;
        }
    }

    GameObject() { };
    virtual ~GameObject() {};
    BoundingSphere *bs;

};


#endif