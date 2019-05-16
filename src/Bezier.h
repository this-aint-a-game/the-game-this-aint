#ifndef BEZIER_H
#define BEZIER_H

#include "glm/glm.hpp"

class Bezier
{
    public:

    // used when you want a quadratic curve (one control point)
    static glm::vec3 quadBez(glm::vec3 (*interpolation)(glm::vec3, glm::vec3, float),
                        glm::vec3 a, 
                        glm::vec3 b, 
                        glm::vec3 control, 
                        float t);


    // two control points, for S-shaped curves or whatever you want
    static glm::vec3 cubeBez(glm::vec3 (*interpolation)(glm::vec3, glm::vec3, float), 
                        glm::vec3 a, 
                        glm::vec3 b, 
                        glm::vec3 control1,
                        glm::vec3 control2, 
                        float t);

};

#endif