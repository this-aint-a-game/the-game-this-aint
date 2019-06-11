
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

    glm::mat4 & getLS();

    GLuint getDepthMap();

    void init(int w, int h);

    void render(Butterfly & butterfly, ObjectCollection *oc, MatrixStack* view, MatrixStack* projection, glm::vec3 camera, GLuint buffer);

    glm::mat4 SetOrthoMatrix();

    glm::mat4 SetLightView(glm::vec3 lightPos, glm::vec3 LA, glm::vec3 up);

};


#endif //OBTAIN_SHADOW_H
