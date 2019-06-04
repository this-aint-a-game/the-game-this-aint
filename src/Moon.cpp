//
// Created by Caroline Cullen on 2019-06-02.
//

#include "Moon.h"
#include "string.h"

using namespace glm;

void Moon::setUp()
{
    // Initialize the GLSL program.
    moonProg = std::make_shared<Program>();
    moonProg->setVerbose(true);
    moonProg->setShaderNames(
            "../resources/moon_vert.glsl",
            "../resources/moon_frag.glsl");
    if (! moonProg->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    moonProg->addUniform("P");
    moonProg->addUniform("V");
    moonProg->addUniform("M");
    moonProg->addUniform("MatAmb");
    moonProg->addUniform("MatDif");
    moonProg->addUniform("MatSpec");
    moonProg->addUniform("shine");
    moonProg->addUniform("view");
    moonProg->addAttribute("vertPos");
    moonProg->addAttribute("vertNor");
    moonProg->addUniform("lightPos");

}
void Moon::initMoon()
{
    this->currentPos.x = 65.f;
    this->currentPos.y = 3.f;
    this->currentPos.z = 65.f;
    this->scale = vec3(3.f);

    uploadMultipleShapes();

    setUp();
}

void Moon::drawObject(MatrixStack* view, MatrixStack* proj, glm::vec3 camerapos, glm::vec3 butterflyPos, ColorCollectGameplay* ccg)
{
    moonProg->bind();
    auto Model = std::make_shared<MatrixStack>();
    Model->pushMatrix();
//    Model->loadIdentity();
    Model->translate(glm::vec3(this->currentPos.x + butterflyPos.x, this->currentPos.y + 0.5f, this->currentPos.z+ butterflyPos.z));
//    Model->translate(glm::vec3(this->currentPos.x, this->currentPos.y + 0.5f, this->currentPos.z));
    Model->scale(this->scale);

    CHECKED_GL_CALL(glUniformMatrix4fv(moonProg->getUniform("P"), 1, GL_FALSE, value_ptr(proj->topMatrix())));
    CHECKED_GL_CALL(glUniformMatrix4fv(moonProg->getUniform("V"), 1, GL_FALSE, value_ptr(view->topMatrix())));
    CHECKED_GL_CALL(glUniformMatrix4fv(moonProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix())));

    for (size_t j = 0; j < moonShapes.size(); j++) {
        if (ccg->checkColor(0)) {
            vec3 ambient = ccg->redColor.ambient;
            vec3 diffuse = ccg->redColor.diffuse;
            vec3 specular = ccg->redColor.specular;
            CHECKED_GL_CALL(glUniform3f(moonProg->getUniform("MatAmb"), ambient.x, ambient.y, ambient.z));
            CHECKED_GL_CALL(glUniform3f(moonProg->getUniform("MatDif"), diffuse.x, diffuse.y, diffuse.z));
            CHECKED_GL_CALL(glUniform3f(moonProg->getUniform("MatSpec"), specular.x, specular.y, specular.z));
            CHECKED_GL_CALL(glUniform1f(moonProg->getUniform("shine"), 4.0));

        } else {
            CHECKED_GL_CALL(glUniform3f(moonProg->getUniform("MatAmb"), 0.13, 0.13, 0.14));
            CHECKED_GL_CALL(glUniform3f(moonProg->getUniform("MatDif"), 0.3, 0.3, 0.4));
            CHECKED_GL_CALL(glUniform3f(moonProg->getUniform("MatSpec"), 0.3, 0.3, 0.4));
            glUniform1f(moonProg->getUniform("shine"), 4.0);
        }

        CHECKED_GL_CALL(glUniform3f(moonProg->getUniform("view"), camerapos.x, camerapos.y, camerapos.z));
        CHECKED_GL_CALL(
                glUniform3f(moonProg->getUniform("lightPos"), currentPos.x + 10.f, currentPos.y, currentPos.z + 10.f));
        moonShapes[j]->draw(moonProg);
    }

    Model->popMatrix();
    moonProg->unbind();
}


void Moon::setPosition(float x, float z)
{
    currentPos.x = x;
    currentPos.z = z;
    currentPos.y = 0.f;
}

void Moon::uploadMultipleShapes()
{

    std::vector<tinyobj::shape_t> TOshapes;
    std::vector<tinyobj::material_t> objMaterials;
    std::string errStr;
    bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                               ("../resources/sphere.obj"));

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

            moonmin = Gmin;
            moonmax = Gmax;
            moonShapes.push_back(s);

        }
    }
}