//
// Created by Caroline Cullen on 2019-05-27.
//

#include "ViewFrustumCulling.h"

int ViewFrustumCulling::ViewFrustCull(glm::vec3 center, float radius) {

    float dist;
    for (int i=0; i < 6; i++) {
        dist = DistToPlane(planes[i].x, planes[i].y, planes[i].z, planes[i].w, center);
        //test against each plane
        if(dist<radius)
            return 1;

    }
    return 0;
}

void ViewFrustumCulling::ExtractVFPlanes(glm::mat4 P, glm::mat4 V) {

    /* composite matrix */
    glm::mat4 comp = P*V;
    glm::vec3 n; //use to pull out normal
    float l; //length of normal for plane normalization

    Left.x = comp[0][3] + comp[0][0];
    Left.y = comp[1][3] + comp[1][0];
    Left.z = comp[2][3] + comp[2][0];
    Left.w = comp[3][3] + comp[3][0];
    n = glm::vec3(Left.x, Left.y, Left.z);
    l = length(n);
    planes[0] = Left/l;
//    std::cout << "Left' " << Left.x << " " << Left.y << " " <<Left.z << " " << Left.w << std::endl;

    Right.x = comp[0][3] - comp[0][0];
    Right.y = comp[1][3] - comp[1][0];
    Right.z = comp[2][3] - comp[2][0];
    Right.w = comp[3][3] - comp[3][0];
    n = glm::vec3(Right.x, Right.y, Right.z);
    l = length(n);
    planes[1] = Right/l;
//    std::cout << "Right " << Right.x << " " << Right.y << " " <<Right.z << " " << Right.w << std::endl;

    Bottom.x = comp[0][3] + comp[0][1];
    Bottom.y = comp[1][3] + comp[1][1];
    Bottom.z = comp[2][3] + comp[2][1];
    Bottom.w = comp[3][3] + comp[3][1];
    n = glm::vec3(Bottom.x, Bottom.y, Bottom.z);
    l = length(n);
    planes[2] = Bottom/l;
//    std::cout << "Bottom " << Bottom.x << " " << Bottom.y << " " <<Bottom.z << " " << Bottom.w << std::endl;

    Top.x = comp[0][3] - comp[0][1];
    Top.y = comp[1][3] - comp[1][1];
    Top.z = comp[2][3] - comp[2][1];
    Top.w = comp[3][3] - comp[3][1];
    n = glm::vec3(Top.x, Top.y, Top.z);
    l = length(n);
    planes[3] = Top/l;
//    std::cout << "Top " << Top.x << " " << Top.y << " " <<Top.z << " " << Top.w << std::endl;

    Near.x = comp[0][3] + comp[0][2];
    Near.y = comp[1][3] + comp[1][2];
    Near.z = comp[2][3] + comp[2][2];
    Near.w = comp[3][3] + comp[3][2];
    n = glm::vec3(Near.x, Near.y, Near.z);
    l = length(n);
    planes[4] = Near/l;
//    std::cout << "Near " << Near.x << " " << Near.y << " " <<Near.z << " " << Near.w << std::endl;

    Far.x = comp[0][3] - comp[0][2];
    Far.y = comp[1][3] - comp[1][2];
    Far.z = comp[2][3] - comp[2][2];
    Far.w = comp[3][3] - comp[3][2];
    n = glm::vec3(Far.x, Far.y, Far.z);
    l = length(n);
    planes[5] = Far/l;
//    std::cout << "Far " << Far.x << " " << Far.y << " " <<Far.z << " " << Far.w << std::endl;
}