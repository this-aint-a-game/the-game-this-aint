#include "Strawberry.h"
#include "Terrain.h"

Strawberry::Strawberry(glm::vec3 min, glm::vec3 max, int num, objType type, ColorCollectGameplay* ccg)
{
    this->currentPos.x = 0.f;
    this->currentPos.z = 0.f;
    this->scale = glm::vec3(3.0f,3.0f,3.0f);
    collected = false;

    this->bs = new BoundingSphere(min, max, this->scale);
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

void Strawberry::drawObject(MatrixStack* Model, std::vector<std::shared_ptr<Shape>> strawberryShapes, std::shared_ptr<Program> prog, glm::vec3 view, glm::vec3 butterflyPos, ColorCollectGameplay* ccg)
{
	Model->pushMatrix();
	Model->translate(glm::vec3(this->currentPos.x, this->currentPos.y + 0.5f, this->currentPos.z));
	Model->scale(this->scale);
	for (size_t j = 0; j < strawberryShapes.size(); j++)
	{
		if(!(this->collected))
		{
			if(j == 0)
			{
				SetMaterial(this->color, prog.get());
			}
			else
			{
				glUniform3f(prog.get()->getUniform("MatAmb"), 0.25f, 0.20725f, 0.20725f);
				glUniform3f(prog.get()->getUniform("MatDif"),1.0f, 0.829f, 0.829f);
				glUniform3f(prog.get()->getUniform("MatSpec"), 0.296648f, 0.296648f, 0.296648f);
				glUniform1f(prog.get()->getUniform("shine"), 12.0f);
			}

		}
        glUniform3f(prog->getUniform("view"), view.x, view.y, view.z);
		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
		glUniform3f(prog->getUniform("lightPos"), butterflyPos.x, butterflyPos.y, butterflyPos.z);
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
