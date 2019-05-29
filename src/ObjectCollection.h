//
// Created by Caroline Cullen on 2019-05-21.
//

#ifndef OBJECTCOLLECTION_H
#define OBJECTCOLLECTION_H

#include "Crystal.h"
#include "Strawberry.h"
#include "Shape.h"
#include "ColorCollectGameplay.h"
#include "BoundingVolumeHierarchy.h"
#include "Plant.h"
#include "Player.h"

class ObjectCollection {

public:

    shared_ptr<Program> objProg;

    Player player = Player();
    ColorCollectGameplay * gameplay = new ColorCollectGameplay();
    BoundingVolumeHierarchy *bvh = new BoundingVolumeHierarchy();

    vector<GameObject*> objects;
    int numPlants;

    // Shape to be used (from obj file)
//    vector<shared_ptr<Shape>> crystal1Shapes;
//    glm::vec3 cryst1min = glm::vec3(0);
//    glm::vec3 cryst1max = glm::vec3(0);
    vector<shared_ptr<Shape>> plantShapes;
    glm::vec3 plantmin = glm::vec3(0);
    glm::vec3 plantmax = glm::vec3(0);
    vector<shared_ptr<Shape>> strawberryShapes;
    glm::vec3 strawMin = glm::vec3(0);
    glm::vec3 strawMax = glm::vec3(0);

    ObjectCollection() {};
    ~ObjectCollection() {};
    // function prototype declarations
    void objectSetUp();
    void initSceneCollectibles();
    void initSceneObjects();
    void uploadMultipleShapes(string objDir, int switchNum);
    void initObjectHierarchy();
};


#endif //THE_GAME_THIS_AINT_OBJECTCOLLECTION_H
