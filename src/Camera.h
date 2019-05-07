#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/glm.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

#define ZOOM_SENSITIVITY 3
#define ANGLE_SENSITIVITY 0.008
#define CAMERA_SPRING 3
#define MAX_DISTANCE 12
#define MIN_DISTANCE 0.5
#define MAX_PITCH -100
#define MIN_PITCH 100

class Camera
{
public:  
    int zoomIn;
    int zoomOut;
    float targetYaw; 
    float yawAngle;
    float targetDistance;
    float distance;
    float pitchAngle;
    glm::vec3 position;

    Camera()
    {
        targetYaw = yawAngle = 0.0;
        targetDistance = distance = 5.0;
    }

    glm::mat4 update
    (     
        glm::vec3 playerPos,
        double frametime, 
        int mousex, 
        int mousey,
        int width, 
        int height
    );

    glm::vec3 getPosition();
};