//
// Created by Caroline Cullen on 2019-06-11.
//

#ifndef THE_GAME_THIS_AINT_PLANK_H
#define THE_GAME_THIS_AINT_PLANK_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/glm.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "GameObject.h"

#include "Lighting.h"

class Plank {

    public:

        std::shared_ptr<Program> plankProg;
        std::vector<std::shared_ptr<Shape>> plankShapes;

    glm::vec3 plankmin = glm::vec3(0);
    glm::vec3 plankmax = glm::vec3(0);

        std::shared_ptr<Texture> texture0;

        glm::vec3 position, scale;

        Plank() {

            position = glm::vec3(-9.43903, -3.4f, 10.66477);

        }

        void uploadMultipleShapes();

        void init();
        void draw(MatrixStack* View, MatrixStack* Projection, glm::vec3 view, Lighting* lighting, glm::vec3 butterflyPos);

        void drawShape(std::shared_ptr<Program> prog);

};


#endif //THE_GAME_THIS_AINT_PLANK_H
