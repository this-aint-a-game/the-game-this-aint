#include "Crystal.h"

using namespace glm;

Crystal::Crystal()
{
    this->currentPos.x = 0.f;
    this->currentPos.z = 0.f;
    this->scale = vec3(0.007f,0.007f,0.007f);
}

void Crystal::drawObject(MatrixStack* Model, std::vector<std::shared_ptr<Shape>> crystalShapes, std::shared_ptr<Program> prog)
{
    Model->pushMatrix();
    Model->translate(vec3(this->currentPos.x, -1.7f, this->currentPos.z));
    Model->rotate(4.5f, vec3(1,0,0));
    Model->scale(this->scale);
    for (size_t j = 0; j < crystalShapes.size(); j++)
    {
            if(j == 0)
            {
                SetMaterial(this->color, prog.get());
            }
            else if (j == 2)
            {
                SetMaterial(8, prog.get());
            }
            else
            {
                SetMaterial(6, prog.get());
            }

        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
        crystalShapes[j]->draw(prog);
    }
    Model->popMatrix();
}

void Crystal::initObject(glm::vec3 min, glm::vec3 max, int num, objType type)
{
    this->bb = new BoundingBox(min, max);
    this->type = type;

    currentPos.x = getRand(-GROUND_SIZE+0.1f, GROUND_SIZE-0.1f);
    currentPos.z = getRand(-GROUND_SIZE, GROUND_SIZE);

    switch(num)
    {
        case 0:
            //red
            color = 7;
            break;
        case 1:
            //orange
            color = 12;
            break;
        case 2:
            //yellow
            color = 11;
            break;
        case 3:
            //green
            color = 13;
            break;
        case 4:
            //blue
            color = 1;
            break;
        case 5:
            //violet
            color = 10;
            break;
    }

}

void Crystal::update(float dt)
{
}

bool Crystal::isCollided(glm::vec3 camera)
{
    return bb->isCollided(camera, currentPos, this->scale);
}

bool Crystal::isCollided(BoundingBox *box)
{
    return bb->isCollided(box, currentPos, this->scale);
}

BoundingBox* Crystal::getBB()
{
    return bb->get(currentPos);
}

