//
// Created by Caroline Cullen on 2019-05-04.
//

#ifndef LIGHTING_H
#define LIGHTING_H

#include <memory>
#include "Program.h"
#include "MatrixStack.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "GLSL.h"
#include <glad/glad.h>
#include <vector>

#include "GLTextureWriter.h"


class Lighting {

public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> intensities;
    GLuint lightTexture;
    float numberLights = 0;

    Lighting();
    void init();
    void bind(GLint handle);
    void unbind();

};


#endif //THE_GAME_THIS_AINT_LIGHTING_H
