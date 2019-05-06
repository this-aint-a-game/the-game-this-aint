//
// Created by Caroline Cullen on 2019-05-05.
//

#ifndef COLORCOLLECTGAMEPLAY_H
#define COLORCOLLECTGAMEPLAY_H


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
            if(red)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if(color ==1)
        {
            if(orange)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if(color ==2)
        {
            if(yellow)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if(color ==3)
        {
            if(green)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if(color ==4)
        {
            if(blue)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else if(color ==5)
        {
            if(violet)
            {
                return true;
            }
            else
            {
                return false;
            }
        } else{
            return false;
        }
    }

};


#endif
