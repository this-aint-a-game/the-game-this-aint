
#ifndef OBTAIN_COLOR_H
#define OBTAIN_COLOR_H

#include <glm/gtc/type_ptr.hpp>

class Color
{
public:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    Color() {}

    Color(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
    {
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
    }

};

#endif //OBTAIN_COLOR_H
