#include <iostream>
#include "Strawberry.h"

#define GROUND_SIZE 120

using namespace glm;

Strawberry::Strawberry()
{
    this->currentPos.x = 0.f;
    this->currentPos.z = 0.f;
    collected = false;
}

float getRand(float min, float max)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * min + r * max;
}

void Strawberry::initObject(glm::vec3 min, glm::vec3 max)
{
	this->bb = new BoundingBox(min, max);

	velocity.x = getRand(-5.f, 5.f);
	velocity.z = getRand(-5.f, 5.f);


	currentPos.x = getRand(-GROUND_SIZE, GROUND_SIZE);
	currentPos.z = getRand(-GROUND_SIZE, GROUND_SIZE);

}

void Strawberry::update(float dt)
{
    if(!collected)
    {
        currentPos += (dt * velocity);
    }
}

bool Strawberry::isCollided(glm::vec3 camera)
{
    return bb->isCollided(camera, currentPos);
}

bool Strawberry::isCollided(BoundingBox *box)
{
	return bb->isCollided(box, currentPos);
}

BoundingBox* Strawberry::getBB()
{
	return bb->get(currentPos);
}

