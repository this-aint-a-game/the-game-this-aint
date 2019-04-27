#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/glm.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

class Camera
{
public:
    glm::vec3 position, yaw, pitch, targetPos, targetYaw, targetPitch;
    int w, a, s, d, r, f;
    Camera()
    {
        w = a = s = d = r = f = 0;
        yaw = pitch = glm::vec3(0, 0, 0);
        position = glm::vec3(0, 0, 0);
        targetPos = targetYaw = targetPitch = glm::vec3(0, 0, 0);
    }
    glm::mat4 process(double frametime, int mousex, int mousey,
                        int width, int height);
};