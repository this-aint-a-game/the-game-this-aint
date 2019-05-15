#include "Camera.h"
#include "Terrain.h"
#include <stdio.h>

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
    targetYaw += YAW_ANGLE_SENSITIVITY * frametime * (width - 4.0 * mousex);
    
    // interpolate towards target distance and yaw
    distance += frametime * CAMERA_SPRING * (targetDistance - distance);
    yawAngle += frametime * CAMERA_SPRING * (targetYaw - yawAngle);

    targetPitchOffset += PITCH_ANGLE_SENSITIVITY * frametime * (height - 4.0 * mousey);
    if(targetPitchOffset < -1 * distance)
        targetPitchOffset = -1 * distance;
    else if(targetPitchOffset > 0.5 * MAX_DISTANCE)
        targetPitchOffset = 0.5 * MAX_DISTANCE;

    pitchOffSet += frametime * CAMERA_SPRING * (targetPitchOffset - pitchOffSet);

    // calculate offsets from the player based on distance and angle
    glm::vec3 offsets = glm::vec3(cos(yawAngle) * distance,
                        pitchOffSet + distance,
                        sin(yawAngle) * distance);
    
    // set the camera position based on player position
    position = playerPos + offsets;

    if(position.y < (Terrain::getHeight(position.x, position.z) + 0.5))
        position.y = Terrain::getHeight(position.x, position.z) + 0.5;

    // orient the camera towards the player once 
    // the camera postion is determined
    return glm::lookAt(position, playerPos, glm::vec3(0,1,0));
}

glm::vec3 Camera::getPosition()
{
    return position;
}
