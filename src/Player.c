#include "Player.h"
#define MAXDEPTH 100
#define MINDEPTH -100
#define MOVESPEED 0.005
#define SENSITIVITY 0.00008

glm::mat4 Player::update(double frametime, int mousex, int mousey,
                            int width, int height)
{
    float speed = 0;
    float lateralSpeed = 0;
    float yawAngle = 0;
    float pitchAngle = 0;

    if (w == 1)
        speed = MOVESPEED * frametime;
    else if (s == 1)
        speed = -MOVESPEED * frametime;
    if(a == 1)
        lateralSpeed = -MOVESPEED * frametime;
    else if(d == 1)
        lateralSpeed = MOVESPEED * frametime;

    /*
    if ((height - 4.0 * mousey) < 0 && targetPitch.x < 3.14159 / 2.0)
        pitchAngle += SENSITIVITY * frametime * (height - 4.0 * mousey);
    else if ((height - 4.0 * mousey) > 0 && targetPitch.x > -3.14159 / 2.0)
        pitchAngle += SENSITIVITY * frametime * (height - 4.0 * mousey);

    yawAngle = SENSITIVITY * frametime * (width - 4.0 * mousex);
   */

    targetYaw.y -= yawAngle; 
    targetPitch.x -= pitchAngle;

    if(targetPitch.x < -3.14159 / 2.0)
        targetPitch.x = -3.14159 / 2.0;
    else if(targetPitch.x > 3.14159 / 2.0)
        targetPitch.x = 3.14159 / 2.0;

    yaw += -0.1f * yaw + 0.1f * targetYaw;
    pitch += -0.1f * pitch + 0.1f * targetPitch;

    glm::mat4 Ry = glm::rotate(glm::mat4(1), yaw.y, glm::vec3(0, 1, 0));
    glm::mat4 Rx = glm::rotate(glm::mat4(1), pitch.x, glm::vec3(1, 0, 0));
    glm::vec4 dir = glm::vec4(0, 0, speed, 1);

    dir = dir * Rx * Ry;
    
    glm::vec3 lateralDir = glm::vec3(glm::vec4(0,0, lateralSpeed, 1) * 
                glm::rotate(glm::mat4(1), 3.14159f / 2.0f, glm::vec3(0,1,0))
                           *Ry);
    targetPos += lateralDir;
    targetPos += glm::vec3(dir.x, dir.y, dir.z);
    position += -0.1f * position + 0.1f * targetPos;

    if (targetPos.y > MAXDEPTH)
        targetPos.y = MAXDEPTH;
    else if (targetPos.y < MINDEPTH)
        targetPos.y = MINDEPTH;
    
    glm::mat4 T = glm::translate(glm::mat4(1), position);
    return Rx * Ry * T;
}