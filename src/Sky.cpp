
#include "Sky.h"
#include "GameObject.h"

void Sky::initTex()
{
    skyTexture = std::make_shared<Texture>();
    skyTexture->setFilename("../resources/night.jpg");
    skyTexture->init();
    skyTexture->setUnit(0);
    skyTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

    sunTexture = std::make_shared<Texture>();
    sunTexture->setFilename("../resources/night.jpg");
    sunTexture->init();
    sunTexture->setUnit(1);
    sunTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Sky::skySetUp()
{
    sphereShape = std::make_shared<Shape>();
    sphereShape->loadMesh("../resources/sphere.obj");
    sphereShape->resize();
    sphereShape->init();

    skyProg = std::make_shared<Program>();
    skyProg->setVerbose(true);
    skyProg->setShaderNames(
            "../resources/sky_tex_vert.glsl",
            "../resources/sky_tex_frag.glsl");
    if (! skyProg->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    skyProg->addUniform("P");
    skyProg->addUniform("V");
    skyProg->addUniform("M");
    skyProg->addUniform("Texture0");
    skyProg->addUniform("Texture1");
    skyProg->addUniform("lightPos");
    skyProg->addAttribute("vertPos");
    skyProg->addAttribute("vertNor");
    skyProg->addAttribute("vertTex");
}

void Sky::drawSky(MatrixStack* View, MatrixStack* Projection, glm::vec3 lightPos, float time)
{
    auto Model = std::make_shared<MatrixStack>();
    skyProg->bind();

    glm::mat4 newView = View->topMatrix();

    newView[3][0] = 0.0;
    newView[3][1] = 0.0;
    newView[3][2] = 0.0;

    glUniformMatrix4fv(skyProg->getUniform("P"), 1, GL_FALSE, glm::value_ptr(Projection->topMatrix()));
    glUniformMatrix4fv(skyProg->getUniform("V"), 1, GL_FALSE, glm::value_ptr(newView));
    glUniform3f(skyProg->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
    Model->pushMatrix();
    Model->loadIdentity();
    Model->pushMatrix();
    Model->rotate(cos(time), glm::vec3(0,1,0));
    Model->scale(glm::vec3(GROUND_SIZE + 0.f, GROUND_SIZE + 0.f, GROUND_SIZE + 0.f));
    glUniformMatrix4fv(skyProg->getUniform("M"), 1, GL_FALSE, glm::value_ptr(Model->topMatrix()));
    skyTexture->bind(skyProg->getUniform("Texture0"));
    sunTexture->bind(skyProg->getUniform("Texture1"));
    sphereShape->draw(skyProg);
    Model->popMatrix();
    skyTexture->unbind();
    sunTexture->unbind();

    Model->popMatrix();
    skyProg->unbind();
}
