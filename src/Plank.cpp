//
// Created by Caroline Cullen on 2019-06-11.
//

#include "Plank.h"


void Plank::init()
{
    plankProg = std::make_shared<Program>();
    plankProg->setVerbose(true);
    plankProg->setShaderNames(
            "../resources/plank_vert.glsl",
            "../resources/plank_frag.glsl");
    if (! plankProg->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    plankProg->addUniform("P");
    plankProg->addUniform("V");
    plankProg->addUniform("M");
    plankProg->addAttribute("vertPos");
    plankProg->addAttribute("vertNor");
    plankProg->addAttribute("vertTex");
    plankProg->addUniform("butterflyPos");
    plankProg->addUniform("normalTex");

    texture0 = std::make_shared<Texture>();
    texture0->setFilename("../resources/wood.bmp");
    texture0->init();
    texture0->setUnit(0);
    texture0->setWrapModes(GL_REPEAT, GL_REPEAT);


    // Initialize the obj mesh VBOs etc
    uploadMultipleShapes();

    scale = glm::vec3(0.008,0.008,0.008);

}

void Plank::draw(MatrixStack* View, MatrixStack* Projection, glm::vec3 view, Lighting* lighting, glm::vec3 butterflyPos)
{
    plankProg->bind();


    auto Model = std::make_shared<MatrixStack>();
    Model->pushMatrix();
    Model->loadIdentity();
    Model->pushMatrix();

    Model->translate(position);
    Model->rotate(80, glm::vec3(1,0,0));
    Model->scale(this->scale);

    for (size_t j = 0; j < plankShapes.size(); j++) {
        glUniformMatrix4fv(plankProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
        glUniformMatrix4fv(plankProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
        glUniformMatrix4fv(plankProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()));
        glUniform3f(plankProg->getUniform("butterflyPos"), butterflyPos.x, butterflyPos.y, butterflyPos.z);
        texture0->bind(plankProg->getUniform("normalTex"));
        /*
           glUniform3f(playerProg->getUniform("view"), view.x, view.y, view.z);
           glUniform3f(playerProg->getUniform("MatAmb"), 0.13, 0.13, 0.14);
           glUniform3f(playerProg->getUniform("MatDif"), 0.3, 0.3, 0.4);
           glUniform3f(playerProg->getUniform("MatSpec"), 0.3, 0.3, 0.4);
           glUniform1f(playerProg->getUniform("shine"), 47.0);
           glUniform1f(playerProg->getUniform("numberLights"), lighting->numberLights);
           lighting->bind(playerProg->getUniform("lighting"));
       */
        plankShapes[j]->draw(plankProg);
    }

    Model->popMatrix();
    plankProg->unbind();
}

void Plank::uploadMultipleShapes()
{

    std::vector<tinyobj::shape_t> TOshapes;
    std::vector<tinyobj::material_t> objMaterials;
    std::string errStr;
    bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                               ("../resources/plank.obj"));

    if (!rc)
    {
        std::cerr << errStr << std::endl;
    }
    else
    {
        glm::vec3 Gmin, Gmax;
        Gmin = glm::vec3(std::numeric_limits<float>::max());
        Gmax = glm::vec3(-std::numeric_limits<float>::max());
        for (size_t i = 0; i < TOshapes.size(); i++)
        {
            std::shared_ptr<Shape> s =  std::make_shared<Shape>();
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

            plankmin = Gmin;
            plankmax = Gmax;
            plankShapes.push_back(s);

        }
    }
}


