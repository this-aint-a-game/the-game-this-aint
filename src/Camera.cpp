#include "Camera.h"
#define MOVESPEED 0.005
#define SENSITIVITY 0.00008
#define CAMERA_SPRING 0.1
#define CAMERA_DISTANCE 5

glm::mat4 Camera::update(double frametime, vec3 playerPos,
                        int mousex, int mousey,
                        int width, int height)
{
    target += SENSITIVITY * frametime * (width - 4.0 * mousex);

    angle += -CAMERA_SPRING * frametime * angle 
            + CAMERA_SPRING * frametime * target;

    glm::vec3 offsets = vec3(cos(angle) * CAMERA_DISTANCE,
                        CAMERA_DISTANCE,
                        sin(angle) * CAMERA_DISTANCE);
    
    glm::vec3 position = playerPos + offsets;

    return glm::lookAt(position, playerPos, glm::vec3(0,1,0));
}