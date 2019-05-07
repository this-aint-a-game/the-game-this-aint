#include "Camera.h"
#include <stdio.h>
#define ZOOM_SENSITIVITY 3
#define ANGLE_SENSITIVITY 0.008
#define CAMERA_SPRING 3
#define MAX_DISTANCE 12
#define MIN_DISTANCE 0.5
#define MAX_PITCH -100
#define MIN_PITCH 100

glm::mat4 Camera::update(glm::vec3 playerPos, double frametime, 
                        int mousex, int mousey,
                        int width, int height)
{
    // adjust target zoom if the requested zoom is within allowed range
    if(zoomIn && targetDistance > MIN_DISTANCE)
        targetDistance -= ZOOM_SENSITIVITY * frametime;
    else if (zoomOut && targetDistance < MAX_DISTANCE)
        targetDistance += ZOOM_SENSITIVITY * frametime;

    // adjust target yaw based on mouse inputs. No constraints. 
    targetYaw += ANGLE_SENSITIVITY * frametime * (width - 4.0 * mousex);

    // interpolate towards target
    distance += frametime * CAMERA_SPRING * (targetDistance - distance);
    yawAngle += frametime * CAMERA_SPRING * (targetYaw - yawAngle);

    // calculate offsets from the player based on distance and angle
    glm::vec3 offsets = glm::vec3(cos(yawAngle) * distance,
                        distance,
                        sin(yawAngle) * distance);
    
    // set the camera position based on player position
    position = playerPos + offsets;

    // orient the camera towards the player once 
    // the camera postion is determined
    return glm::lookAt(position, playerPos, glm::vec3(0,1,0));
}

glm::vec3 Camera::getPosition()
{
    return position;
}
