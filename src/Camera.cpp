#include "Camera.h"
#include <stdio.h>
#define MOVESPEED 0.005
#define SENSITIVITY 0.008
#define CAMERA_SPRING 5
#define CAMERA_DISTANCE 2

glm::mat4 Camera::update(glm::vec3 playerPos, double frametime, 
                        int mousex, int mousey,
                        int width, int height)
{
    target += SENSITIVITY * frametime * (width - 4.0 * mousex);

    angle += frametime * CAMERA_SPRING * (target - angle);

    glm::vec3 offsets = glm::vec3(cos(angle) * CAMERA_DISTANCE,
                        CAMERA_DISTANCE,
                        sin(angle) * CAMERA_DISTANCE);
    
    glm::vec3 position = playerPos + offsets;

    return glm::lookAt(position, playerPos, glm::vec3(0,1,0));
}