#pragma once

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#define GROUND_SIZE 120

#include <string>
#include <vector>
#include "BoundingBox.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include <glm/gtc/type_ptr.hpp>

class GameObject
{

public:
    enum objType{strawberry, crystal1, crystal2, crystal3};
    virtual void initObject(glm::vec3, glm::vec3, int, objType) = 0;
    virtual void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>, std::shared_ptr<Program>) = 0;
    virtual void update(float) = 0;
    virtual bool isCollided(glm::vec3) = 0;
    virtual bool isCollided(BoundingBox*) = 0;
    virtual BoundingBox* getBB() = 0;

    glm::vec3 currentPos;
    glm::vec3 scale;
    int color;
    objType type;

    void SetMaterial(int i, Program *prog)
    {
        switch (i)
        {

            case 0: // flat grey
                glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
                glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
                glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
                glUniform1f(prog->getUniform("shine"), 4.0);
                break;
            case 1: // shiny blue plastic
                glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
                glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
                glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
                glUniform1f(prog->getUniform("shine"), 120.0);
                break;
            case 2: // brass
                glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
                glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
                glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
                glUniform1f(prog->getUniform("shine"), 27.9);
                break;
            case 3: // pearl
                glUniform3f(prog->getUniform("MatAmb"), 0.25f, 0.20725f, 0.20725f);
                glUniform3f(prog->getUniform("MatDif"), 1.0f, 0.829f, 0.829f);
                glUniform3f(prog->getUniform("MatSpec"), 0.296648f, 0.296648f, 0.296648f);
                glUniform1f(prog->getUniform("shine"), 11.264f);
                break;
            case 4: // copper
                glUniform3f(prog->getUniform("MatAmb"),  0.19125f, 0.0735f, 0.0225f);
                glUniform3f(prog->getUniform("MatDif"), 0.7038f, 0.27048f, 0.0828f);
                glUniform3f(prog->getUniform("MatSpec"), 0.256777f, 0.137622f, 0.086014f);
                glUniform1f(prog->getUniform("shine"), 12.8f);
                break;
            case 5: // turqoise
                glUniform3f(prog->getUniform("MatAmb"),  0.1f, 0.18725f, 0.1745f);
                glUniform3f(prog->getUniform("MatDif"), 0.396f, 0.74151f, 0.69102f);
                glUniform3f(prog->getUniform("MatSpec"), 0.297254f, 0.30829f, 0.306678f);
                glUniform1f(prog->getUniform("shine"), 12.8f);
                break;
            case 6: // obisdian
                glUniform3f(prog->getUniform("MatAmb"),  0.05375f, 0.05f, 0.06625f);
                glUniform3f(prog->getUniform("MatDif"), 0.18275f, 0.17f, 0.22525f);
                glUniform3f(prog->getUniform("MatSpec"), 0.332741f, 0.328634f, 0.346435f);
                glUniform1f(prog->getUniform("shine"), 38.4f);
                break;
            case 7: // ruby
                glUniform3f(prog->getUniform("MatAmb"),  0.1745f, 0.01175f, 0.01175f);
                glUniform3f(prog->getUniform("MatDif"), 0.61424f, 0.04136f, 0.04136f);
                glUniform3f(prog->getUniform("MatSpec"), 0.727811f, 0.626959f, 0.626959f);
                glUniform1f(prog->getUniform("shine"), 76.8f);
                break;
            case 8: // emerald
                glUniform3f(prog->getUniform("MatAmb"),  0.0215f, 0.1745f, 0.0215f);
                glUniform3f(prog->getUniform("MatDif"), 0.07568f, 0.61424f, 0.07568f);
                glUniform3f(prog->getUniform("MatSpec"), 0.633f, 0.727811f, 0.633f);
                glUniform1f(prog->getUniform("shine"), 76.8f);
                break;
            case 9: // yellow plastic
                glUniform3f(prog->getUniform("MatAmb"),  0.05f,0.05f,0.0f);
                glUniform3f(prog->getUniform("MatDif"), 0.5f,0.5f,0.4f);
                glUniform3f(prog->getUniform("MatSpec"), 0.7f,0.7f,0.04f);
                glUniform1f(prog->getUniform("shine"), 10.0f);
                break;

            case 10: // blueberries
                glUniform3f(prog->getUniform("MatAmb"),  0.105882f, 0.058824f, 0.313725f);
                glUniform3f(prog->getUniform("MatDif"), 0.227451f, 0.270588f, 0.741176f);
                glUniform3f(prog->getUniform("MatSpec"), 0.233333f, 0.233333f, 0.821569f);
                glUniform1f(prog->getUniform("shine"), 9.84615f);
                break;

            case 11: // lemon
                glUniform3f(prog->getUniform("MatAmb"), 0.4294, 0.4235, 0.02745);
                glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
                glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
                glUniform1f(prog->getUniform("shine"), 27.9);
                break;

            case 12: // orange
                glUniform3f(prog->getUniform("MatAmb"),  0.49125f, 0.135f, 0.0225f);
                glUniform3f(prog->getUniform("MatDif"), 0.8038f, 0.37048f, 0.0828f);
                glUniform3f(prog->getUniform("MatSpec"), 0.556777f, 0.537622f, 0.286014f);
                glUniform1f(prog->getUniform("shine"), 12.8f);
                break;

            case 13: // lime
                glUniform3f(prog->getUniform("MatAmb"),  0.0215f, 0.1745f, 0.0215f);
                glUniform3f(prog->getUniform("MatDif"), 0.07568f, 0.61424f, 0.07568f);
                glUniform3f(prog->getUniform("MatSpec"), 0.633f, 0.727811f, 0.633f);
                glUniform1f(prog->getUniform("shine"), 76.8f);
                break;

            case 15: // banana
                glUniform3f(prog->getUniform("MatAmb"),  0.05f,0.05f,0.0f);
                glUniform3f(prog->getUniform("MatDif"), 0.5f,0.5f,0.4f);
                glUniform3f(prog->getUniform("MatSpec"), 0.7f,0.7f,0.04f);
                glUniform1f(prog->getUniform("shine"), 35.0f);
                break;

            case 16: // pith
                glUniform3f(prog->getUniform("MatAmb"), 0.7294, 0.7235, 0.2745);
                glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
                glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
                glUniform1f(prog->getUniform("shine"), 27.9);
                break;

            case 17: // chrome
                glUniform3f(prog->getUniform("MatAmb"), 0.25f, 0.25f, 0.25f);
                glUniform3f(prog->getUniform("MatDif"), 0.4f, 0.4f, 0.4f);
                glUniform3f(prog->getUniform("MatSpec"), 0.774597f, 0.774597f, 0.774597f);
                glUniform1f(prog->getUniform("shine"), 76.8f);
                break;

            case 18: // black plastic
                glUniform3f(prog->getUniform("MatAmb"), 0.0f, 0.0f, 0.0f);
                glUniform3f(prog->getUniform("MatDif"), 0.01f, 0.01f, 0.01f);
                glUniform3f(prog->getUniform("MatSpec"), 0.50f, 0.50f, 0.50f);
                glUniform1f(prog->getUniform("shine"), 32.0f);
                break;

        }
    }

    float getRand(float min, float max)
    {
        float r = rand() / (float) RAND_MAX;
        return (1.0f - r) * min + r * max;
    }


    GameObject() {};
    virtual ~GameObject() {};

protected:
    BoundingBox *bb;

};

#endif