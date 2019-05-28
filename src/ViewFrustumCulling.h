//
// Created by Caroline Cullen on 2019-05-27.
//

#ifndef VIEWFRUSTUMCULLING_H
#define VIEWFRUSTUMCULLING_H

#include <vector>
#include <memory>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Player.h"
#include "GameObject.h"
#include "Program.h"


class ViewFrustumCulling{
public:
    glm::vec4 Left, Right, Bottom, Top, Near, Far;
    glm::vec4 planes[6];

    float DistToPlane(float A, float B, float C, float D, glm::vec3 point) {

        return (A*point.x + B*point.y + C*point.z + D);
    }
    void ExtractVFPlanes(glm::mat4 P, glm::mat4 V);
    int ViewFrustCull(glm::vec3 center, float radius);

};


#endif //VIEWFRUSTUMCULLING_H
