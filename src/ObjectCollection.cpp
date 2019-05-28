//
// Created by Caroline Cullen on 2019-05-21.
//

#include "ObjectCollection.h"
void ObjectCollection::initObjectHierarchy()
{
    bvh->init(objects);
}

void ObjectCollection::objectSetUp()
{
    // Initialize the GLSL program.
    objProg = make_shared<Program>();
    objProg->setVerbose(true);
    objProg->setShaderNames(
            "../resources/phong_vert.glsl",
            "../resources/phong_frag.glsl");
    if (! objProg->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    objProg->addUniform("P");
    objProg->addUniform("V");
    objProg->addUniform("M");
    objProg->addUniform("MatAmb");
    objProg->addUniform("MatDif");
    objProg->addUniform("MatSpec");
    objProg->addUniform("shine");
    objProg->addUniform("view");
    objProg->addAttribute("vertPos");
    objProg->addAttribute("vertNor");
    objProg->addUniform("lighting");
    objProg->addUniform("numberLights");
    objProg->addUniform("lightPos");

}

void ObjectCollection::initSceneCollectibles()
{
    uploadMultipleShapes("/mushroom.obj", 0);

    // For now, hard code purple prize close to start
    Strawberry * first = new Strawberry(strawMin, strawMax, 5, GameObject::strawberry, gameplay);
    first->setPosition(-8, 20);
    objects.push_back(first);
    gameplay->setPos(first->color, first->currentPos);

    // For now, hard code blue prize close to start
    Strawberry * second = new Strawberry(strawMin, strawMax, 4, GameObject::strawberry, gameplay);
    second->setPosition(8, 8);
    objects.push_back(second);
    gameplay->setPos(second->color, second->currentPos);


    for(int i = 0; i < 4; i++)
    {
        bool found_spot = false;
        Strawberry *berry;
        while (!found_spot)
        {
            berry = new Strawberry(strawMin, strawMax, i, GameObject::strawberry, gameplay);
            BoundingSphere *otherBS = berry->getBS();
            for (int j = 0; j < objects.size(); j++)
            {
                berry = new Strawberry(strawMin, strawMax, i, GameObject::strawberry, gameplay);
                BoundingSphere *otherBS = berry->getBS();

                if ((objects[j]->isCollided(otherBS)))
                {
                    delete berry;
                    delete otherBS;
                }
            }
            found_spot = true;
            delete otherBS;
        }
        objects.push_back(berry);
        gameplay->setPos(berry->color, berry->currentPos);
    }
}

void ObjectCollection::initSceneObjects()
{
    uploadMultipleShapes("/plant.obj", 1);
    numPlants = glm::clamp(rand() % 100, 50, 100);

    for(int i = 0; i < numPlants; i++)
    {
        bool found_spot = false;
        Plant *plant;
        while (!found_spot)
        {
            int num = glm::clamp(rand() % 6, 0, 6);
            plant = new Plant(plantmin, plantmax, num, GameObject::plant, gameplay);
            BoundingSphere *otherBS = plant->getBS();
            for (int j = 0; j < objects.size(); j++)
            {
                plant = new Plant(plantmin, plantmax, num, GameObject::plant, gameplay);
                BoundingSphere *otherBS = plant->getBS();

                if ((objects[j]->isCollided(otherBS)))
                {
                    delete plant;
                    delete otherBS;
                }
            }
            found_spot = true;
            delete otherBS;
        }
        objects.push_back(plant);
    }

}

//GameObject::objType selectRandomCrystal()
//{
//    int random = rand() % 3;
//
//    if(random == 0)
//    {
//        return GameObject::crystal1;
//    }
//    else if (random == 1)
//    {
//        return GameObject::crystal2;
//    }
//    else
//    {
//        return GameObject::crystal3;
//    }
//
//}

void ObjectCollection::uploadMultipleShapes(string objDir, int switchNum)
{

    vector<tinyobj::shape_t> TOshapes;
    vector<tinyobj::material_t> objMaterials;
    string errStr;
    bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                               ("../resources" + objDir).c_str());

    if (!rc)
    {
        cerr << errStr << endl;
    }
    else
    {
        glm::vec3 Gmin, Gmax;
        Gmin = glm::vec3(std::numeric_limits<float>::max());
        Gmax = glm::vec3(-std::numeric_limits<float>::max());
        for (size_t i = 0; i < TOshapes.size(); i++)
        {
            shared_ptr<Shape> s =  make_shared<Shape>();
            s->createShape(TOshapes[i]);
            s->measure();

            if(s->min.x < Gmin.x)
            {
                Gmin.x = s->min.x;
            }

            if(s->max.x > Gmax.x)
            {
                Gmax.x = s->max.x;
            }

            if(s->min.y < Gmin.y)
            {
                Gmin.y = s->min.y;
            }

            if(s->max.y > Gmax.y)
            {
                Gmax.y = s->max.y;
            }

            if(s->min.z < Gmin.z)
            {
                Gmin.z = s->min.z;
            }

            if(s->max.z > Gmax.z)
            {
                Gmax.z = s->max.z;
            }

            s->init();


            switch (switchNum)
            {
                case 0:
                    strawMin = Gmin;
                    strawMax = Gmax;
                    strawberryShapes.push_back(s);
                    break;
                case 1:
                    plantmax = Gmax;
                    plantmin = Gmin;
                    plantShapes.push_back(s);
            }
        }
    }
}