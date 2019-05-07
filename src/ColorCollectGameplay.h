//
// Created by Caroline Cullen on 2019-05-05.
//

#ifndef COLORCOLLECTGAMEPLAY_H
#define COLORCOLLECTGAMEPLAY_H

#include <iostream>
#include "Color.h"


class ColorCollectGameplay {

public:

    bool red;
    bool orange;
    bool yellow;
    bool green;
    bool blue;
    bool violet;

    Color redColor;
    Color orangeColor;
    Color yellowColor;
    Color greenColor;
    Color blueColor;
    Color violetColor;

    ColorCollectGameplay()
    {
        red = false;
        orange = false;
        yellow = false;
        green = false;
        blue = false;
        violet = false;

        redColor = Color(glm::vec3(0.666, 0.262, 0.258),
                    glm::vec3(0.766, 0.362, 0.358),
                    glm::vec3(0.866, 0.462, 0.458)); // shine 47

        orangeColor = Color(glm::vec3(0.833, 0.482, 0.003),
                       glm::vec3(0.933, 0.582, 0.103),
                       glm::vec3(0.933, 0.682, 0.403));

        yellowColor = Color(glm::vec3(0.560, 0.509, 0.239),
                       glm::vec3(0.610, 0.559, 0.289),
                       glm::vec3(0.760, 0.709, 0.439));

        greenColor = Color(glm::vec3(0.425, 0.545, 0.117),
                      glm::vec3(0.425, 0.545, 0.117),
                      glm::vec3(0.725, 0.745, 0.317));

        blueColor = Color(glm::vec3(0.049, 0.576, 0.617),
                     glm::vec3(0.049, 0.576, 0.617),
                     glm::vec3(0.049, 0.676, 0.717));

        violetColor = Color(glm::vec3(0.445f, 0.194f, 0.488f),
                       glm::vec3(0.445f, 0.194f, 0.488f),
                       glm::vec3(0.545f, 0.294f, 0.588f));

    }

    void collectRed(){red = true;};
    void collectOrange(){orange = true;};
    void collectYellow(){yellow = true;};
    void collectGreen(){green = true;};
    void collectBlue(){blue = true;};
    void collectViolet(){violet = true;};

    bool checkColor(int color)
    {
        if(color == 0)
        {
            return red;
        }
        else if(color ==1)
        {
            return orange;
        }
        else if(color ==2)
        {
            return yellow;
        }
        else if(color ==3)
        {
            return green;
        }
        else if(color ==4)
        {
            return blue;
        }
        else if(color ==5)
        {
            return violet;
        } else{
            return false;
        }
    }

};


#endif
