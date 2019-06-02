//
// Created by Caroline Cullen on 2019-06-02.
//

#include "Moon.h"
using namespace glm;
Moon::Moon(glm::vec3 min, glm::vec3 max, int num, objType type, ColorCollectGameplay * ccg)
{
    this->currentPos.x = 65.f;
    this->currentPos.y = 3.f;
    this->currentPos.z = 65.f;
    this->scale = vec3(0.03f);

    this->bs = new BoundingSphere(min, max, this->scale, 0.3f);
    this->type = type;
    this->ccg = ccg;
    this->color = 0;
}

void Moon::drawObject(MatrixStack* Model, std::vector<std::shared_ptr<Shape>> moonShapes, std::shared_ptr<Program> prog, glm::vec3 view, glm::vec3 butterflyPos, ColorCollectGameplay* ccg)
{
    Model->pushMatrix();
    Model->translate(glm::vec3(this->currentPos.x + butterflyPos.x, this->currentPos.y + 0.5f, this->currentPos.z+ butterflyPos.z));
    Model->scale(this->scale);
    for (size_t j = 0; j < moonShapes.size(); j++)
    {
        if(ccg->checkColor(this->color)) {
            if (j == 0) {
                SetMaterial(this->color, prog.get());
            } else {
                glUniform3f(prog.get()->getUniform("MatAmb"), 0.25f, 0.20725f, 0.20725f);
                glUniform3f(prog.get()->getUniform("MatDif"), 1.0f, 0.829f, 0.829f);
                glUniform3f(prog.get()->getUniform("MatSpec"), 0.296648f, 0.296648f, 0.296648f);
                glUniform1f(prog.get()->getUniform("shine"), 12.0f);
            }
        } else {
            glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
            glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
            glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
            glUniform1f(prog->getUniform("shine"), 4.0);
        }

        glUniform3f(prog->getUniform("view"), view.x, view.y, view.z);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
        glUniform3f(prog->getUniform("lightPos"), currentPos.x - 4.f, currentPos.y, currentPos.z - 4.f);
        moonShapes[j]->draw(prog);

    }
    Model->popMatrix();
}


void Moon::setPosition(float x, float z)
{
    currentPos.x = x;
    currentPos.z = z;
    currentPos.y = 0.f;
}