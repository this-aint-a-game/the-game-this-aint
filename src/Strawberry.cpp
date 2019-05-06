#include "Strawberry.h"
#include "Terrain.h"

Strawberry::Strawberry()
{
    this->currentPos.x = 0.f;
    this->currentPos.z = 0.f;
    this->scale = glm::vec3(1.0f,1.0f,1.0f);
    collected = false;
}

void Strawberry::drawObject(MatrixStack* Model, std::vector<std::shared_ptr<Shape>> strawberryShapes, std::shared_ptr<Program> prog)
{
	Model->pushMatrix();
	Model->translate(glm::vec3(this->currentPos.x, this->currentPos.y + 0.5f, this->currentPos.z));
//	Model->rotate(glfwGetTime()/2, vec3(0,1,0));
	Model->scale(this->scale);
	for (size_t j = 0; j < strawberryShapes.size(); j++)
	{
		if(!(this->collected))
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
		}
		else
		{
			SetMaterial(6, prog.get());
		}
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		strawberryShapes[j]->draw(prog);
	}
	Model->popMatrix();
}
void Strawberry::initObject(glm::vec3 min, glm::vec3 max, int num, objType type)
{
	this->bb = new BoundingBox(min, max);
	this->type = type;

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


int Strawberry::collect()
{
	return this->color;
}

void Strawberry::update(float dt)
{
}

bool Strawberry::isCollided(glm::vec3 camera)
{
    return bb->isCollided(camera, currentPos, this->scale);
}

bool Strawberry::isCollided(BoundingBox *box)
{
	return bb->isCollided(box, currentPos, this->scale);
}

BoundingBox* Strawberry::getBB()
{
	return bb->get(currentPos);
}

