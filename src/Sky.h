
#ifndef OBTAIN_SKY_H
#define OBTAIN_SKY_H

#include "Program.h"
#include "Texture.h"
#include "Shape.h"
#include "MatrixStack.h"
#include <iostream>

#include <memory>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include "GLSL.h"

class Sky
{
    std::shared_ptr<Program> skyProg;
    std::shared_ptr<Texture> skyTexture;
    std::shared_ptr<Texture> sunTexture;
    std::shared_ptr<Shape> sphereShape;

public:
    void initTex();
    void skySetUp();
    void drawSky(MatrixStack* View, MatrixStack* Projection, glm::vec3 lightPos, float time);

};

#endif //OBTAIN_SKY_H
