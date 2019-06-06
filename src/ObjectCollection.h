//
// Created by Caroline Cullen on 2019-05-21.
//

#ifndef OBJECTCOLLECTION_H
#define OBJECTCOLLECTION_H

#include "Strawberry.h"
#include "Shape.h"
#include "Camera.h"
#include "ColorCollectGameplay.h"
#include "BoundingVolumeHierarchy.h"
#include "Plant.h"
#include "Player.h"
#include <irrKlang.h>

class ObjectCollection {

public:

    shared_ptr<Program> objProg;

    Player player = Player();
    ColorCollectGameplay * gameplay = new ColorCollectGameplay();
    BoundingVolumeHierarchy *bvh = new BoundingVolumeHierarchy();

    vector<GameObject*> objects;
    vector<GameObject*> plants;

    int numPlants;

    // Shape to be used (from obj file)
    vector<shared_ptr<Shape>> plantShapes;
    glm::vec3 plantmin = glm::vec3(0);
    glm::vec3 plantmax = glm::vec3(0);
    vector<shared_ptr<Shape>> strawberryShapes;
    glm::vec3 strawMin = glm::vec3(0);
    glm::vec3 strawMax = glm::vec3(0);



    void setSoundEngine(irrklang::ISoundEngine* soundEngine)
    {
        bvh->setSoundEngine(soundEngine);
    }

    void setCamera(Camera* camera)
    {
        bvh->setCamera(camera);
    }

    ~ObjectCollection() {};
    // function prototype declarations
    void objectSetUp();
    void initSceneCollectibles();
    void initSceneObjects();
    void uploadMultipleShapes(string objDir, int switchNum);
    void initObjectHierarchy();
    void drawScene(shared_ptr<Program> prog, MatrixStack* View, MatrixStack* Projection, glm::vec3 camera, glm::vec3 butterfly);
};


#endif //THE_GAME_THIS_AINT_OBJECTCOLLECTION_H
