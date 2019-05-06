#include "Camera.h"
#include <stdio.h>
#define MOVESPEED 0.05
#define SENSITIVITY 0.008
#define CAMERA_SPRING 5
#define CAMERA_DISTANCE 1

glm::mat4 Camera::update(glm::vec3 playerPos, double frametime, 
                        int mousex, int mousey,
                        int width, int height)
{
    target += SENSITIVITY * frametime * (width - 4.0 * mousex);

    angle += frametime * CAMERA_SPRING * (target - angle);

    glm::vec3 offsets = glm::vec3(cos(angle) * CAMERA_DISTANCE,
                        CAMERA_DISTANCE,
                        sin(angle) * CAMERA_DISTANCE);
    
    position = playerPos + offsets;

    return glm::lookAt(position, playerPos, glm::vec3(0,1,0));
}

glm::vec3 Camera::getPosition()
{
    return position;
}
