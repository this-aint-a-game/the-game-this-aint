#include "Strawberry.h"
#include "Terrain.h"

Strawberry::Strawberry(glm::vec3 min, glm::vec3 max, int num, objType type, ColorCollectGameplay* ccg)
{
    this->currentPos.x = 0.f;
    this->currentPos.z = 0.f;
    this->scale = glm::vec3(1.0f,1.0f,1.0f);
    collected = false;

    this->bb = new BoundingBox(min, max);
    this->type = type;
    this->ccg = ccg;

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

    setPosition(getRand(-GROUND_SIZE, GROUND_SIZE), getRand(-GROUND_SIZE, GROUND_SIZE));
}

void Strawberry::drawObject(MatrixStack* Model, std::vector<std::shared_ptr<Shape>> strawberryShapes, std::shared_ptr<Program> prog, glm::vec3 view)
{
	Model->pushMatrix();
	Model->translate(glm::vec3(this->currentPos.x, this->currentPos.y + 0.5f, this->currentPos.z));
//	Model->rotate(glfwGetTime()/2, vec3(0,1,0));
	Model->scale(this->scale);
	for (size_t j = 0; j < strawberryShapes.size(); j++)
	{
		if(!(this->collected))
		{
		    SetMaterial(this->color, prog.get());
		}
		else
		{
			SetMaterial(6, prog.get());
		}
        glUniform3f(prog->getUniform("view"), view.x, view.y, view.z);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		strawberryShapes[j]->draw(prog);
	}
	Model->popMatrix();
}

void Strawberry::setPosition(float x, float z)
{
	currentPos.x = x;
	currentPos.z = z;
	currentPos.y = Terrain::getHeight(currentPos.x, currentPos.z);
}


void Strawberry::collect()
{
    switch(color)
	{
		case 0:
			ccg->collectRed();
			break;
		case 1:
			ccg->collectOrange();
			break;
		case 2:
			ccg->collectYellow();
			break;
		case 3:
			ccg->collectGreen();
			break;
		case 4:
			ccg->collectBlue();
			break;
		case 5:
			ccg->collectViolet();
			break;
	}
}
