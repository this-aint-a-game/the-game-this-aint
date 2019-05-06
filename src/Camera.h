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
    glm::vec3 position;

    Camera()
    {
        target = angle = 0.0;
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