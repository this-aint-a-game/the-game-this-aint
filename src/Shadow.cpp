//
// Created by Alexa Drenick on 2019-06-03.
//

#include "Shadow.h"


glm::mat4 & Shadow::getLS()
{
    return LS;
}

GLuint Shadow::getDepthMap()
{
    return depthMap;
}

void Shadow::init(int w, int h)
{
    this->width = w;
    this->height = h;

    depthProg = std::make_shared<Program>();
    depthProg->setVerbose(true);
    depthProg->setShaderNames("../resources/depth_vert.glsl", "../resources/depth_frag.glsl");
    //depthProg->init();
    if (! depthProg->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    depthProg->addUniform("LP");
    depthProg->addUniform("LV");
    depthProg->addUniform("M");
    depthProg->addAttribute("vertPos");

    // these aren't used, it's just to make drawing game objects into depth buffer simple
    depthProg->addUniform("shine");
    depthProg->addUniform("lightPos");
    depthProg->addUniform("view");
    depthProg->addUniform("MatSpec");
    depthProg->addUniform("MatAmb");
    depthProg->addUniform("MatDif");
    depthProg->addUniform("P");
    depthProg->addUniform("V");

    glGenFramebuffers(1, &depthMapFBO);

    //generate the texture
    glGenTextures(1, &depthMap); // TODO?
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //bind with framebuffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void Shadow::render(Butterfly & butterfly, ObjectCollection *oc, MatrixStack* view, MatrixStack* projection, glm::vec3 camera, GLuint buffer, ViewFrustumCulling* vfc)
{
    //glViewport(0, 0, S_WIDTH, S_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT); // TODO?

    //set up shadow shader
    //render scene
    depthProg->bind();
    glm::mat4 LP = SetOrthoMatrix();
    glm::vec3 lightPos = butterfly.currentPos;
    t += 0.001; // TODO frametime
    glm::mat4 LV = SetLightView(lightPos, oc->player.currentPos, glm::vec3(1, 0, 0));
    LS = LP * LV;

    oc->player.drawShape(depthProg);
    oc->drawScene(depthProg, view, projection, camera, butterfly.currentPos, vfc);

    depthProg->unbind();
    glCullFace(GL_BACK);  // TODO?

    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
}

glm::mat4 Shadow::SetOrthoMatrix()
{
    // orthographic view volume - numbers set up extents
    glm::mat4 ortho = glm::ortho(-7.f, 7.f, -7.f, 7.0f, 0.1f, 15.f);

    glUniformMatrix4fv(depthProg->getUniform("LP"), 1, GL_FALSE, value_ptr(ortho));
    return ortho;
}

glm::mat4 Shadow::SetLightView(glm::vec3 lightPos, glm::vec3 LA, glm::vec3 up)
{
    glm::mat4 Cam = glm::lookAt(lightPos, LA, up);
    glUniformMatrix4fv(depthProg->getUniform("LV"), 1, GL_FALSE, value_ptr(Cam));
    return Cam;
}



