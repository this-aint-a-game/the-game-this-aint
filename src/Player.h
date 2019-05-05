#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/glm.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

class Player
{
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
    glm::mat4 update
    (
        double frametime, 
        int mousex, 
        int mousey,
        int width, 
        int height
    );
};