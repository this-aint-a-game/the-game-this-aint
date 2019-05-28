#pragma once

#ifndef PARTICLE_H_INCLUDED
#define PARTICLE_H_INCLUDED

#include <vector>
#include <memory>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Player.h"
#include "GameObject.h"
#include "Program.h"

using namespace glm;

class MatrixStack;
class Program;
class Texture;


class Particle
{

public:

	void load();
	void rebirth(float t, glm::vec3 playPos, ColorCollectGameplay* ccg);
    float randFloat(float l, float h);
	void update(float t, float h, const glm::vec3 &g, const bool *keyToggles, glm::vec3, ColorCollectGameplay*);
    glm::vec3 x = glm::vec3(randFloat(-75.f, 75.f), randFloat(-75.f, 75.f), randFloat(-75.f, 75.f));
    glm::vec4 color = glm::vec4(1.f);


private:

	glm::vec3 v = glm::vec3(0.f);
	float lifespan = 1.f;
	float tEnd = 0.f;
	float scale = 1.f;


};

class ParticleSorter
{

public:

	bool operator() (const std::shared_ptr<Particle> p0, const std::shared_ptr<Particle> p1) const
	{
		const vec3 &x0 = p0->x;
		const vec3 &x1 = p1->x;

		vec4 x0w = C * glm::vec4(x0.x, x0.y, x0.z, 1.0f);
		vec4 x1w = C * glm::vec4(x1.x, x1.y, x1.z, 1.0f);
		return x0w.z < x1w.z;
	}

	mat4 C;

};

#endif
