#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <glm/glm.hpp>
#include <stdlib.h>
#include "BoundingSphere.h"
#include "glm/gtc/matrix_transform.hpp"


class BoundingBox
{

public:

    glm::vec3 bbmin;
    glm::vec3 bbmax;
    std::vector<GameObject*> objects_contained;

    BoundingBox* right_box = nullptr;
    BoundingBox* left_box = nullptr;

    bool isCollided(BoundingSphere* otherBB)
    {
        glm::vec3 curmin = this->bbmin;
        glm::vec3 curmax = this->bbmax;

        if(!(((otherBB->midpt.x) > curmin.x) && ((otherBB->midpt.x) < curmax.x)))
        {
            return true;
        }
        else if(!(((otherBB->midpt.z) > curmin.z) && ((otherBB->midpt.z) < curmax.z)))
        {
            return true;
        }
        else
        {
            return false;
        }

    };

    BoundingBox() {};

    BoundingBox(glm::vec3 min, glm::vec3 max)
    {
        bbmin = glm::vec3(glm::vec4(min, 1));
        bbmax = glm::vec3(glm::vec4(max, 1));
    };

    BoundingBox* get(glm::vec3 pos)
    {
        glm::vec3 curmin = pos + bbmin;
        glm::vec3 curmax = pos + bbmax;
        return new BoundingBox(curmin, curmax);
    }

};

#endif