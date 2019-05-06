#pragma once

#ifndef COLLECTIBLEOBJECT_H
#define COLLECTIBLEOBJECT_H

#include "GameObject.h"

class CollectibleObject : public GameObject
{

public:

    bool collected;
    virtual void collect() = 0;

    CollectibleObject() {};
    virtual ~CollectibleObject() {};

};

#endif