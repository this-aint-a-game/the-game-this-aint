#ifndef PLAYER_H
#define PLAYER_H

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

#define MOVESPEED 2
#define PLAYER_ROTATION_SPRING 10
#define PI 3.14159

class Player : public GameObject
{
    std::shared_ptr<Program> playerProg;
    std::shared_ptr<Shape>   playerShape;
    glm::mat4 model;

    glm::mat4 updateModelMatrix
            (
                    double frametime,
                    int mousex,
                    int mousey,
                    int width,
                    int height
            );

public:
    glm::vec3 oldPos, position, targetPos;
    float yaw, targetYaw;
    bool w, a, s, d;
    // for determining if the player can move up/left/down/right
    bool uFree, lFree, dFree, rFree;
    
    Player()
    {
        w = a = s = d = false;
        uFree = lFree = dFree = rFree = true;
        yaw = targetYaw = 0.0f;
        position = glm::vec3(-8.43903, 0, 9.66477);
        oldPos = targetPos = position;
    }

    void updateView(double frametime, int mousex, int mousey, int width, int height);
    void updateFreeDirs(BoundingBox* otherBB);

    void initPlayer(ColorCollectGameplay * ccg);
    void drawPlayer(MatrixStack* View, MatrixStack* Projection, glm::mat4 & LS, GLuint depthMap, glm::vec3 view, Lighting* lighting);
    // TODO this should be less specific
    void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>, std::shared_ptr<Program>, glm::vec3 view) {};

    bool checkForCollisions(std::vector<GameObject*> & objs);

    void drawShape(std::shared_ptr<Program> prog);
};

#endif