//
// Created by Caroline Cullen on 2019-06-02.
//

#ifndef MOON_H
#define MOON_H
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/glm.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "GameObject.h"
#include "ColorCollectGameplay.h"
#include "Lighting.h"

class Moon{
public:

    std::shared_ptr<Program> moonProg;
    glm::vec3 currentPos;
    std::vector<std::shared_ptr<Shape>> moonShapes;
    glm::vec3 moonmin = glm::vec3(0);
    glm::vec3 moonmax = glm::vec3(0);
    glm::vec3 scale;

    void uploadMultipleShapes();
    void drawObject(MatrixStack*,  MatrixStack*, glm::vec3 view, glm::vec3, ColorCollectGameplay* ccg);
    void setPosition(float, float);
    void setUp();
    void initMoon();
    Moon(){};
    virtual ~Moon() {};
};


#endif //THE_GAME_THIS_AINT_MOON_H
