#ifndef BUTTERFLY_H
#define BUTTERFLY_H

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
#include "Bezier.h"

class Butterfly
{
    public:
    float angle;
    glm::vec3 offsets;
    glm::vec3 center;
    std::shared_ptr<Program> butterflyProg;
    std::shared_ptr<Shape>   butterflyShape;
    std::shared_ptr<Shape>   butterflyShapeWingsUp;
    glm::mat4 model;
    glm::vec3 currentPos;
    glm::vec3 scale;

    Butterfly()
    {
        angle = 0;
    }

    void updateModelMatrix(double frametime, glm::vec3 origin);
    void initbutterfly();
    void drawbutterfly(std::shared_ptr<Program> prog, MatrixStack* View, MatrixStack* Projection, glm::vec3 view, ColorCollectGameplay*);
    void moveAlongPath(glm::vec3 a, glm::vec3 b, glm::vec3 control1, glm::vec3 control2, double frametime, double t);
    // TODO this should be less specific
    void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>, std::shared_ptr<Program>, glm::vec3 view) {};
    void drawShape(std::shared_ptr<Program> prog);
};

#endif

