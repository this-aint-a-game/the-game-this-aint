#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/glm.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

class Camera
{
public:
    float angle;
    float target;
    Camera()
    {
        target = angle = 0.0;
    }
    glm::mat4 update
    (
        double frametime, 
        vec3 playerPos,
        int mousex, 
        int mousey,
        int width, 
        int height
    );
};