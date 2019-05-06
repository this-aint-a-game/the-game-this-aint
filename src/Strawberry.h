#pragma once

#ifndef STRAWBERRY_H
#define STRAWBERRY_H

#include "CollectibleObject.h"

using namespace std;

class Strawberry : public CollectibleObject
{

public:

	void initObject(glm::vec3, glm::vec3, int, objType, ColorCollectGameplay*);
	void update(float);
	bool isCollided(glm::vec3);
	bool isCollided(BoundingBox*);
	void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>,  std::shared_ptr<Program>);
    BoundingBox* getBB();
    int collect();

	Strawberry();
	virtual ~Strawberry() {};

private:


};

#endif