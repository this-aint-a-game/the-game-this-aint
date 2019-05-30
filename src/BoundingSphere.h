//
// Created by Caroline Cullen on 2019-05-18.
//

#ifndef BOUNDINGSPHERE_H
#define BOUNDINGSPHERE_H

#include <glm/glm.hpp>
#include <stdlib.h>
#include <iostream>

#include "glm/gtc/matrix_transform.hpp"
#define PADDING 0.3

class BoundingSphere {

public:

    glm::vec3 midpt;
    float radius;

    glm::vec3 sphere_min;
    glm::vec3 sphere_max;

    float distance(glm::vec3 A, glm::vec3 B)
    {
        return sqrt(((A.x-B.x)*(A.x-B.x)) + ((A.y-B.y)*(A.y-B.y)) + ((A.z-B.z)*(A.z-B.z)));
    }

    bool isCollided(BoundingSphere* otherSphere, glm::vec3 currentPos)
    {
        glm::vec3 curSpherePos = this->midpt + currentPos;
        glm::vec3 otherCurPos = otherSphere->midpt;

        float dist = distance(curSpherePos, otherCurPos);
        float sumOfRadius = otherSphere->radius + this->radius;

        if(dist < sumOfRadius)
        {
            return true;
        }
        else
        {
            return false;
        }

    };

    BoundingSphere() {};

    BoundingSphere(glm::vec3 min, glm::vec3 max, glm::vec3 scale_vec)
    {
        // not based on the current position of the sphere, only the local min and max of the object size
        // but scaled by a factor specified by the world
        glm::mat4 sm = glm::scale(glm::mat4(1), scale_vec);
        this->sphere_min = glm::vec3(glm::vec4(min, 1) * sm);
        this->sphere_max = glm::vec3(glm::vec4(max, 1) * sm);
        this->midpt = ((sphere_max + sphere_min) / 2.f);
        this->radius = distance(sphere_max, midpt) - PADDING;
    };

    BoundingSphere* get(glm::vec3 pos)
    {
        glm::vec3 min = pos + this->sphere_min;
        glm::vec3 max = pos + this->sphere_max;

        return new BoundingSphere(min, max, glm::vec3(1.f));
    }
};


#endif //BOUNDINGSPHERE_H
