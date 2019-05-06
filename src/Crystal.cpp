#include "Crystal.h"
#include "Terrain.h"

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
    Model->translate(vec3(this->currentPos.x, this->currentPos.y, this->currentPos.z));
    Model->rotate(4.5f, vec3(1,0,0));
    Model->scale(this->scale);

    for (size_t j = 0; j < crystalShapes.size(); j++)
    {
        if(ccg->checkColor(this->color))
        {
            SetMaterial(this->color, prog.get());
        }
        else
        {
            glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
            glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
            glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
            glUniform1f(prog->getUniform("shine"), 4.0);
        }

        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
        crystalShapes[j]->draw(prog);
    }
    Model->popMatrix();
}
//glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
//		    glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
//		    glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
//		    glUniform1f(prog->getUniform("shine"), 4.0);
//break;
void Crystal::initObject(glm::vec3 min, glm::vec3 max, int num, objType type, ColorCollectGameplay* ccg)
{
    this->bb = new BoundingBox(min, max);
    this->type = type;
    this->ccg = ccg;

    currentPos.x = getRand(-GROUND_SIZE+0.1f, GROUND_SIZE-0.1f);
    currentPos.z = getRand(-GROUND_SIZE, GROUND_SIZE);
    currentPos.y = Terrain::getHeight(currentPos.x, currentPos.z);

    switch(num)
    {
        case 0:
            //red
            color = 0;
            break;
        case 1:
            //orange
            color = 1;
            break;
        case 2:
            //yellow
            color = 2;
            break;
        case 3:
            //green
            color = 3;
            break;
        case 4:
            //blue
            color = 4;
            break;
        case 5:
            //violet
            color = 5;
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

