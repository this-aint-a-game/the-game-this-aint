//
// Created by Caroline Cullen on 2019-05-27.
//

#include "Plant.h"
#include "Terrain.h"
#include "glm/gtc/matrix_transform.hpp"

using namespace glm;

float randFloat(float l, float h)
{
    float r = rand() / (float) RAND_MAX;
    return (1.0f - r) * l + r * h;
}

Plant::Plant(glm::vec3 min, glm::vec3 max, int num, objType type, ColorCollectGameplay * ccg)
{
    this->currentPos.x = 0.f;
    this->currentPos.z = 0.f;
    this->scale = vec3(randFloat(0.05f, 0.1f));

    this->bs = new BoundingSphere(min, max, this->scale, 0.3f);
    this->type = type;
    this->ccg = ccg;
    this->color = 3;


    currentPos.x = getRand(-GROUND_SIZE+0.1f, GROUND_SIZE-0.1f);
    currentPos.z = getRand(-GROUND_SIZE, GROUND_SIZE);
    while(Terrain::getHeight(currentPos.x, currentPos.z)> -4.5f)
    {
        currentPos.x = getRand(-GROUND_SIZE+0.1f, GROUND_SIZE-0.1f);
        currentPos.z = getRand(-GROUND_SIZE, GROUND_SIZE);
    }

    currentPos.y = (Terrain::getHeight(currentPos.x, currentPos.z)) - 0.35f;
}

void Plant::drawObject(MatrixStack* Model, std::vector<std::shared_ptr<Shape>> plantShapes, std::shared_ptr<Program> prog, glm::vec3 view, glm::vec3 butterflyPos, ColorCollectGameplay* ccg)
{
    Model->pushMatrix();
    Model->translate(glm::vec3(this->currentPos.x, this->currentPos.y + 0.5f, this->currentPos.z));
    Model->scale(this->scale);
    for (size_t j = 0; j < plantShapes.size(); j++)
    {
        if(ccg->checkColor(this->color)) {

            SetMaterial(this->color, prog.get());

        } else {
            glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
            glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
            glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
            glUniform1f(prog->getUniform("shine"), 4.0);
        }

            glUniform3f(prog->getUniform("view"), view.x, view.y, view.z);
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
            glUniform3f(prog->getUniform("lightPos"), butterflyPos.x, butterflyPos.y, butterflyPos.z);
            plantShapes[j]->draw(prog);

    }
    Model->popMatrix();
}


void Plant::setPosition(float x, float z)
{
    currentPos.x = x;
    currentPos.z = z;
    currentPos.y = Terrain::getHeight(currentPos.x, currentPos.z);
}

