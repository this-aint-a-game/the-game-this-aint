

#include <iostream>
#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"
#include "WindowManager.h"
#include "ColorCollectGameplay.h"


float randFloat(float l, float h)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * l + r * h;
}

void Particle::load()
{
}

float getDistance(glm::vec3 playPos, glm::vec3 otherPos)
{
    return sqrt(((playPos.x-otherPos.x)*(playPos.x-otherPos.x)) + ((playPos.z-otherPos.z)*(playPos.z-otherPos.z)));
}

int getClosestColor(glm::vec3 playPos, ColorCollectGameplay* ccg)
{
    float closestDis = 50000000000000.f;
    int closestColor = 0;

    if((closestDis>getDistance(playPos, ccg->redColorPos)) && !ccg->red)
    {
        closestDis = getDistance(playPos, ccg->redColorPos);
    }

    if((closestDis>getDistance(playPos, ccg->orangeColorPos))  && !ccg->orange)
    {
        closestDis = getDistance(playPos, ccg->orangeColorPos);
        closestColor = 1;
    }

    if((closestDis>getDistance(playPos, ccg->yellowColorPos))  && !ccg->yellow)
    {
        closestDis = getDistance(playPos, ccg->yellowColorPos);
        closestColor = 2;
    }

    if((closestDis>getDistance(playPos, ccg->greenColorPos))  && !ccg->green)
    {
        closestDis = getDistance(playPos, ccg->greenColorPos);
        closestColor = 3;
    }

    if((closestDis>getDistance(playPos, ccg->blueColorPos))  && !ccg->blue)
    {
        closestDis = getDistance(playPos, ccg->blueColorPos);
        closestColor = 4;
    }

    if((closestDis>getDistance(playPos, ccg->violetColorPos))  && !ccg->violet)
    {
        closestDis = getDistance(playPos, ccg->violetColorPos);
        closestColor = 5;
    }

    return closestColor;
}

void Particle::rebirth(float t, glm::vec3 playPos, ColorCollectGameplay* ccg)
{
	x.x = playPos.x;
    x.y = playPos.y;
	x.z = playPos.z;

    int colorSelected = getClosestColor(playPos, ccg);

    if((colorSelected == 0) && (!ccg->red))
    {
        v.x = (ccg->redColorPos.x - playPos.x)+ randFloat(-0.5f, 0.5f);
        v.y = (ccg->redColorPos.y - playPos.y) + randFloat(0.1f, 0.5f);
        v.z = (ccg->redColorPos.z - playPos.z)+ randFloat(-0.5f, 0.5f);

        color.r = ccg->redColor.diffuse.r + randFloat(-0.1f, 0.1f);
        color.g = ccg->redColor.diffuse.g + randFloat(-0.1f, 0.1f);
        color.b = ccg->redColor.diffuse.b + randFloat(-0.1f, 0.1f);
    }
    else if((colorSelected == 1) && (!ccg->orange))
    {
        v.x = (ccg->orangeColorPos.x - playPos.x)+ randFloat(-0.5f, 0.5f);
        v.y = (ccg->orangeColorPos.y - playPos.y) + randFloat(0.5f, 1.0f);
        v.z = (ccg->orangeColorPos.z - playPos.z)+ randFloat(-0.5f, 0.5f);


        color.r = ccg->orangeColor.diffuse.r + randFloat(-0.1f, 0.1f);
        color.g = ccg->orangeColor.diffuse.g + randFloat(-0.1f, 0.1f);
        color.b = ccg->orangeColor.diffuse.b + randFloat(-0.1f, 0.1f);
    }
    else if((colorSelected == 2) && (!ccg->yellow))
    {
        v.x = (ccg->yellowColorPos.x - playPos.x)+ randFloat(-0.5f, 0.5f);
        v.y = (ccg->yellowColorPos.y - playPos.y) + randFloat(0.1f, 0.5f);
        v.z = (ccg->yellowColorPos.z - playPos.z)+ randFloat(-0.5f, 0.5f);

        color.r = ccg->yellowColor.diffuse.r + randFloat(-0.1f, 0.1f);
        color.g = ccg->yellowColor.diffuse.g + randFloat(-0.1f, 0.1f);
        color.b = ccg->yellowColor.diffuse.b + randFloat(-0.1f, 0.1f);

    }
    else if((colorSelected == 3) && (!ccg->green))
    {
        v.x = (ccg->greenColorPos.x - playPos.x)+ randFloat(-0.5f, 0.5f);
        v.y = (ccg->greenColorPos.y - playPos.y) + randFloat(0.1f, 0.5f);
        v.z = (ccg->greenColorPos.z - playPos.z)+ randFloat(-0.5f, 0.5f);

        color.r = ccg->greenColor.diffuse.r + randFloat(-0.1f, 0.1f);
        color.g = ccg->greenColor.diffuse.g + randFloat(-0.1f, 0.1f);
        color.b = ccg->greenColor.diffuse.b + randFloat(-0.1f, 0.1f);
    }
    else if((colorSelected == 4) && (!ccg->blue))
    {

        v.x = (ccg->blueColorPos.x- playPos.x) + randFloat(-0.5f, 0.5f);
        v.y = (ccg->blueColorPos.y - playPos.y) + randFloat(0.1f, 0.5f);
        v.z = (ccg->blueColorPos.z - playPos.z) + randFloat(-0.5f, 0.5f);


        color.r = ccg->blueColor.diffuse.r + randFloat(-0.1f, 0.1f);
        color.g = ccg->blueColor.diffuse.g + randFloat(-0.1f, 0.1f);
        color.b = ccg->blueColor.diffuse.b + randFloat(-0.1f, 0.1f);
    }
    else if((colorSelected == 5) && (!ccg->violet))
    {

        v.x = (ccg->violetColorPos.x - playPos.x) + randFloat(-0.5f, 0.5f);
        v.y = (ccg->violetColorPos.y - playPos.y) + randFloat(0.1f, 0.5f);
        v.z = (ccg->violetColorPos.z - playPos.z) + randFloat(-0.5f, 0.5f);


        color.r = ccg->violetColor.diffuse.r + randFloat(-0.1f, 0.1f);
        color.g = ccg->violetColor.diffuse.g + randFloat(-0.1f, 0.1f);
        color.b = ccg->violetColor.diffuse.b + randFloat(-0.1f, 0.1f);
    }
    else
    {
        v.x = randFloat(-2.5f, 2.5f);
        v.y = randFloat(-3.0f, -0.5f);
        v.z = randFloat(-2.5f, 2.5f);

        color.r = 0.928f;
        color.g = 0.828f;
        color.b = 0.928f;
    }

    v = glm::normalize(v);
	lifespan = randFloat(2.f, 5.f);
	tEnd = t + lifespan;

	scale = randFloat(0.5f, 10.0f);
	color.a = 1.0f;
}

void Particle::update(float t, float h, const vec3 &g, const bool *keyToggles, glm::vec3 playPos, ColorCollectGameplay* ccg)
{
	if (t > tEnd)
	{
		rebirth(t, playPos, ccg);

	}

	x += h * v;
	color.a = (tEnd - t) / lifespan;
}
