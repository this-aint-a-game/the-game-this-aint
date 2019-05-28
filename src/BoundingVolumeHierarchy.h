//
// Created by Caroline Cullen on 2019-05-21.
//

#ifndef BOUNDINGVOLUMEHIERARCHY_H
#define BOUNDINGVOLUMEHIERARCHY_H

#include "GameObject.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "Strawberry.h"
class BoundingVolumeHierarchy {

public:

    BoundingBox *root;

    // functions
    void init(std::vector<GameObject*>);
    void printBVH(BoundingBox*);
    void createBVH(BoundingBox*, std::vector<GameObject*>);
    bool checkForCollision(std::vector<GameObject*> &, BoundingBox* cur, BoundingSphere* bs);
    GameObject* checkAgainstShapes(BoundingBox* cur, BoundingSphere* bs);
    std::vector<GameObject*> splitArray(std::vector<GameObject*> og_array, int index_begin, int index_end);

};


#endif //BOUNDINGVOLUMEHIERARCHY_H
