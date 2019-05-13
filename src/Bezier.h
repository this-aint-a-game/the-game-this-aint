#ifndef BEZIER_H
#define BEZIER_H

#include "glm/glm.hpp"

using namespace glm;

class Bezier
{
    public:

    // used when you want a quadratic curve (one control point)
    static vec3 quadBez(vec3 (*interpolation)(vec3, vec3, float),
                        vec3 a, 
                        vec3 b, 
                        vec3 control, 
                        float t);


    // two control points, for S-shaped curves or whatever you want
    static vec3 cubeBez(vec3 (*interpolation)(vec3, vec3, float), 
                        vec3 a, 
                        vec3 b, 
                        vec3 control1,
                        vec3 control2, 
                        float t);

};

#endif