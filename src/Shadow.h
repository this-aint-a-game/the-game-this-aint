
#ifndef OBTAIN_SHADOW_H
#define OBTAIN_SHADOW_H

#include "Program.h"
#include "Lighting.h"
#include "Player.h"
#include "Butterfly.h"
#include <glm/gtc/type_ptr.hpp>
#include "ObjectCollection.h"


class Shadow {

    std::shared_ptr<Program> depthProg;
    GLuint depthMapFBO;
    GLuint depthMap;
    int width;
    int height;
    glm::mat4 LS;
    float t;

public:
    Shadow()
    {
        this->t = 0.f;
    }

    glm::mat4 & getLS()
    {
        return LS;
    }

    GLuint getDepthMap()
    {
        return depthMap;
    }

    void init(int w, int h)
    {
        this->width = w;
        this->height = h;

        depthProg = std::make_shared<Program>();
        depthProg->setVerbose(true);
        depthProg->setShaderNames("../resources/depth_vert.glsl", "../resources/depth_frag.glsl");
        depthProg->init();
        depthProg->init();
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //bind with framebuffer's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

    }

    void render(Butterfly & butterfly, ObjectCollection *oc, MatrixStack* view, MatrixStack* projection, glm::vec3 camera)
    {
        //glViewport(0, 0, S_WIDTH, S_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
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
        oc->drawScene(depthProg, view, projection, camera, butterfly.currentPos);

        depthProg->unbind();
        glCullFace(GL_BACK);  // TODO?

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glm::mat4 SetOrthoMatrix()
    {
        // orthographic view volume - numbers set up extents
        //glm::mat4 ortho = glm::ortho(-15.f, 15.f, -15.f, 15.0f, 0.1f, 30.f);
        glm::mat4 ortho = glm::ortho(-7.f, 7.f, -7.f, 7.0f, 0.1f, 15.f);
        //glm::mat4 ortho = glm::ortho(-5.f, 5.f, -5.f, 5.0f, 0.1f, 12.f);

        //fill in the glUniform call to send to the right shader!
        glUniformMatrix4fv(depthProg->getUniform("LP"), 1, GL_FALSE, value_ptr(ortho));
        return ortho;
    }

    glm::mat4 SetLightView(glm::vec3 lightPos, glm::vec3 LA, glm::vec3 up)
    {
        glm::mat4 Cam = glm::lookAt(lightPos, LA, up);
        glUniformMatrix4fv(depthProg->getUniform("LV"), 1, GL_FALSE, value_ptr(Cam));
        return Cam;
    }

};


#endif //OBTAIN_SHADOW_H
