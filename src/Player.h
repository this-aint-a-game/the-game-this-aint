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
    glm::vec3 position, yaw, pitch, targetPos, targetYaw, targetPitch;
    int w, a, s, d;
    Player()
    {
        w = a = s = d = 0;
        yaw = pitch = glm::vec3(0, 0, 0);
        position = glm::vec3(-8.43903, 0, 9.66477);
        targetPos = position;
        targetYaw = targetPitch = glm::vec3(0, 0, 0);
    }

    void updateView(double frametime, int mousex, int mousey, int width, int height);

    void initPlayer(ColorCollectGameplay * ccg);
    void drawPlayer(MatrixStack* View, MatrixStack* Projection, glm::vec3 view);

    // TODO this should be less specific
    void drawObject(MatrixStack*, std::vector<std::shared_ptr<Shape>>, std::shared_ptr<Program>, glm::vec3 view) {};

    bool checkForCollisions(std::vector<GameObject*> & objs);
};