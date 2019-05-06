#pragma once

#ifndef STRAWBERRY_H
#define STRAWBERRY_H

#include "CollectibleObject.h"

using namespace std;

class Strawberry : public CollectibleObject
{

public:

	void initObject(glm::vec3, glm::vec3, int, objType, ColorCollectGameplay* ccg);
	void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>,  std::shared_ptr<Program>, glm::vec3 view);
    void collect();
	void setPosition(float x, float z);

	Strawberry();
	virtual ~Strawberry() {};

private:


};

#endif