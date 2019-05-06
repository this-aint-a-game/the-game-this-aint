//
// Particle random generator
//

#include <iostream>
#include "Particle.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "Texture.h"
#include "WindowManager.h"


float randFloat(float l, float h)
{
	float r = rand() / (float) RAND_MAX;
	return (1.0f - r) * l + r * h;
}

void Particle::load()
{
	// Random initialization
	rebirth(0.0f);
}

// all particles born at the origin
void Particle::rebirth(float t)
{
	charge = randFloat(0.0f, 1.0f) < 0.5f ? -1.0f : 1.0f;
	m = 1.0f;
	d = randFloat(0.0f, 0.02f);
	x.x = randFloat(-400.f, 400.f);
	x.y = randFloat(0.f, 120.f);
	x.z = randFloat(-400.f, 400.f);
	v.x = randFloat(-2.5f, 2.5f);
	v.y = randFloat(-3.0f, -0.5f);
	v.z = randFloat(-2.5f, 2.5f);
	lifespan = randFloat(2.f, 20.f);
	tEnd = t + lifespan;

	scale = randFloat(0.5f, 10.0f);
	color.r = 0.928f;
	color.g = 0.828f;
	color.b = 0.928f;
	color.a = 1.0f;
}

void Particle::update(float t, float h, const vec3 &g, const bool *keyToggles)
{
	if (t > tEnd)
	{
		rebirth(t);

	}

	// very simple update;
	x += h * v;

	// x *= -0.98f;
	//  x.y += -0.8f;

	color.a = (tEnd - t) / lifespan;
}
