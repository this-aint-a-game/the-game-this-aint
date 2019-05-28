#pragma once

#ifndef STRAWBERRY_H
#define STRAWBERRY_H

#include "CollectibleObject.h"

using namespace std;

class Strawberry : public CollectibleObject
{

public:

	void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>,  std::shared_ptr<Program>, glm::vec3 view, glm::vec3, ColorCollectGameplay* ccg);
    void collect();
	void setPosition(float x, float z);

	Strawberry(glm::vec3 min, glm::vec3 max, int color, objType, ColorCollectGameplay * ccg);
	virtual ~Strawberry() {};

private:


};

#endif