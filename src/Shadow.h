
#ifndef OBTAIN_SHADOW_H
#define OBTAIN_SHADOW_H

#include "Program.h"
#include "Lighting.h"
#include "Player.h"
#include "Butterfly.h"
#include <glm/gtc/type_ptr.hpp>


class Shadow {

    std::shared_ptr<Program> depthProg;
    GLuint depthMapFBO;
    GLuint depthMap;
    int width;
    int height;
    Lighting *lighting;
    glm::mat4 LS;
    float t;

public:
    Shadow(Lighting *l)
    {
        this->lighting = l;
        this-> t = 0.f;
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

    void render(Player & player, Butterfly & butterfly)
    {
        //glViewport(0, 0, S_WIDTH, S_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT); // TODO?

        //set up shadow shader
        //render scene
        depthProg->bind();
        glm::mat4 LP = SetOrthoMatrix();
        //LA = glm::vec3(0, 0, 0)
        //glm::vec3 lightPos = player.currentPos + glm::vec3(0, 3, 0);
        //glm::vec3 lightPos = player.currentPos + glm::vec3(2*sin(t), 3, cos(t)*2);
        glm::vec3 lightPos = butterfly.currentPos;
        t += 0.001; // TODO frametime
        //glm::vec3 look = glm::lookAt(lightPos, player.currentPos, glm::vec3(1, 0, 0));
        glm::mat4 LV = SetLightView(lightPos, player.currentPos, glm::vec3(1, 0, 0));
        LS = LP * LV;

        player.drawShape(depthProg);
        depthProg->unbind();
        glCullFace(GL_BACK);  // TODO?

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glm::mat4 SetOrthoMatrix()
    {
        // orthographic view volume - numbers set up extents
        //glm::mat4 ortho = glm::ortho(-15.f, 15.f, -15.f, 15.0f, 0.1f, 30.f);
        //glm::mat4 ortho = glm::ortho(-7.f, 7.f, -7.f, 7.0f, 0.1f, 15.f);
        glm::mat4 ortho = glm::ortho(-5.f, 5.f, -5.f, 5.0f, 0.1f, 12.f);

        //fill in the glUniform call to send to the right shader!
        glUniformMatrix4fv(depthProg->getUniform("LP"), 1, GL_FALSE, value_ptr(ortho)); // LV is uniform in shader
        return ortho;
    }

    glm::mat4 SetLightView(glm::vec3 lightPos, glm::vec3 LA, glm::vec3 up)
    {
        // TODO multiple lights
        //lightPos = lighting->positions.front();

        glm::mat4 Cam = glm::lookAt(lightPos, LA, up);
        glUniformMatrix4fv(depthProg->getUniform("LV"), 1, GL_FALSE, value_ptr(Cam)); // LV is uniform in shader
        //fill in the glUniform call to send to the right shader!
        return Cam;
    }

};


#endif //OBTAIN_SHADOW_H
