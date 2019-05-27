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

#define MOVESPEED 2.0f
#define PLAYER_ROTATION_SPRING 10.0f
#define PI 3.14159f

class Player : public GameObject
{
    public:

    std::shared_ptr<Program> playerProg;
    std::shared_ptr<Shape>   playerShape;
    glm::mat4 model;

    glm::mat4 updateModelMatrix
            (
                    double frametime,
                    int mousex,
                    int mousey,
                    int width,
                    int height,
                    glm::vec3 lastCamDir,
                    std::vector<GameObject*> & objs
            );

    glm::vec3 oldPos, position, dir, targetDir;
    float yaw, targetYaw;
    bool w, a, s, d;
    
    Player()
    {
        w = a = s = d = false;
        yaw = targetYaw = 0.0f;
        position = glm::vec3(-8.43903, 0, 9.66477);
        dir = targetDir = glm::vec3(0,0,0);
        oldPos = position;
    }

    void updateView(double frametime, int mousex, int mousey, int width, int height, glm::vec3 camPos, std::vector<GameObject*> & objs);
    void initPlayer(ColorCollectGameplay * ccg);
    void drawPlayer(MatrixStack* View, MatrixStack* Projection, glm::vec3 view, Lighting* lighting, glm::vec3 butterflyPos);
    // TODO this should be less specific
    void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>, std::shared_ptr<Program>, glm::vec3 view) {};
    bool checkForCollisions(std::vector<GameObject*> & objs);

    void drawShape(std::shared_ptr<Program> prog);
};

#endif