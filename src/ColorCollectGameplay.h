//
// Created by Caroline Cullen on 2019-05-05.
//

#ifndef COLORCOLLECTGAMEPLAY_H
#define COLORCOLLECTGAMEPLAY_H

#include <iostream>


class ColorCollectGameplay {

public:

    bool red = false;
    bool orange = false;
    bool yellow = false;
    bool green = false;
    bool blue = false;
    bool violet = false;

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
