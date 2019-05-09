#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <glm/glm.hpp>
#include <stdlib.h>
#include "glm/gtc/matrix_transform.hpp"


class BoundingBox
{

public:

    glm::vec3 bbmin;
    glm::vec3 bbmax;

    bool isCollided(BoundingBox* otherBB, glm::vec3 currentPos, glm::vec3 scale)
    {
        glm::vec3 curmin = currentPos + bbmin;//*scale);
        glm::vec3 curmax = currentPos + bbmax;//*scale);

        glm::vec3 othermin = otherBB->bbmin;
        glm::vec3 othermax = otherBB->bbmax;


        return ((curmin.x <= othermax.x) && (curmax.x >= othermin.x)) &&
                ((curmin.z <= othermax.z) && (curmax.z >= othermin.z));


    };

    bool isCollided(glm::vec3 camera, glm::vec3 currentPos, glm::vec3 scale)
    {
        glm::vec3 curmin = currentPos + bbmin;//*scale) ;
        glm::vec3 curmax = currentPos + bbmax;//*scale);

        if(!(((camera.x) > curmin.x) && ((camera.x) < curmax.x)))
        {
            return false;
        }
        else if(!(((camera.z) > curmin.z) && ((camera.z) < curmax.z)))
        {
            return false;
        }
        else
        {
            return true;
        }

    };


    BoundingBox() {};

    BoundingBox(glm::vec3 min, glm::vec3 max, glm::vec3 scale_vec)
    {
        glm::mat4 sm = glm::scale(glm::mat4(1), scale_vec);
        bbmin = glm::vec3(glm::vec4(min, 1) * sm);
        bbmax = glm::vec3(glm::vec4(max, 1) * sm);
    };

    BoundingBox* get(glm::vec3 pos)
    {
        glm::vec3 curmin = pos + bbmin;
        glm::vec3 curmax = pos + bbmax;
        return new BoundingBox(curmin, curmax, glm::vec3(1.f));
    }

};

#endif