/*
Alexa
Caroline
Doug
Sam
obtain.
*/

#include <iostream>
#include <glad/glad.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"
#include "WindowManager.h"
#include "GLTextureWriter.h"
#include "Particle.h"
#include "Strawberry.h"
#include "Terrain.h"
#include "Crystal.h"

#define PI 3.1415
#define MOVEMENT_SPEED 0.2f
#define RENDER_SPEED 0.5f

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;
	int width, height;
	std::string resourceDir = "../resources";
	bool red = false;
    bool orange = false;
    bool yellow = false;
    bool green = false;
    bool blue = false;
	bool violet = false;
	vector<GameObject*> objects;

	// programs
	shared_ptr<Program> shapeProg;
	//shared_ptr<Program> groundProg;
	shared_ptr<Program> particleProg;
	shared_ptr<Program> skyProg;

	Terrain *terrain = new Terrain();
	shared_ptr<Texture> skyTexture;
	shared_ptr<Texture> sunTexture;
	shared_ptr<Texture> particleTexture;

	//ground info
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int gGiboLen;

	//transforms for the world
	vec3 worldTrans = vec3(0);
	float worldScale = 1.0;

	// Shape to be used (from obj file)
    shared_ptr<Shape> sphereShape;
    vector<shared_ptr<Shape>> crystal1Shapes;
	glm::vec3 cryst1min = glm::vec3(0);
	glm::vec3 cryst1max = glm::vec3(0);
    vector<shared_ptr<Shape>> crystal2Shapes;
	glm::vec3 cryst2min = glm::vec3(0);
	glm::vec3 cryst2max = glm::vec3(0);
    vector<shared_ptr<Shape>> crystal3Shapes;
	glm::vec3 cryst3min = glm::vec3(0);
	glm::vec3 cryst3max = glm::vec3(0);
    vector<shared_ptr<Shape>> strawberryShapes;
	glm::vec3 strawMin = glm::vec3(0);
	glm::vec3 strawMax = glm::vec3(0);

    int numCrystals;

	//stuff necessary for particles
	vector<std::shared_ptr<Particle>> particles;
	GLuint ParticleVertexArrayID;
	int numP = 600;
	GLfloat points[1800];
	GLfloat pointColors[2400];
	GLuint particlePointsBuffer;
	GLuint particleColorBuffer;
	float t0_disp = 0.0f;
	float t_disp = 0.0f;
	bool keyToggles[256] = { false };
	float t = 0.0f;
	float h = 0.01f;
	glm::vec3 g = glm::vec3(0.0f, -0.01f, 0.0f);


	bool FirstTime = true;
	bool sprint = false;
	bool Moving = false;
	int gMat = 0;

	bool mouseDown = false;

	float phi = 0;
	float theta = 90;
	float prevX;
	float prevY;
	vec3 cameraPos = vec3(0.0, 0.0, 0.0);
	vec3 lightPos = vec3(0, 500.0, 0);

	float x = PI/2;
	float y = 0;
	float z = 0;
	bool moveLeft = false;
	bool moveRight = false;
	bool moveForward = false;
	bool moveBackward = false;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		switch (key) {
			case GLFW_KEY_A:
				moveLeft = (action != GLFW_RELEASE);
				break;
			case GLFW_KEY_D:
				moveRight = (action != GLFW_RELEASE);
				break;
			case GLFW_KEY_W:
				moveForward = (action != GLFW_RELEASE);
				break;
			case GLFW_KEY_S:
				moveBackward = (action != GLFW_RELEASE);
				break;
			case GLFW_KEY_LEFT_SHIFT:
				if (action == GLFW_PRESS) {
					sprint = true;
					break;
				}
				sprint = false;
				break;
			case GLFW_KEY_F:
				if (action != GLFW_RELEASE) {
					terrain->updateFreq(true);
				}
				break;
			case GLFW_KEY_V:
				if (action != GLFW_RELEASE) {
					terrain->updateFreq(false);
				}
				break;
			case GLFW_KEY_O:
				if (action != GLFW_RELEASE) {
					terrain->updateOctave();
				}
				break;
			case GLFW_KEY_P:
				if (action != GLFW_RELEASE) {
					terrain->updatePower(true);
				}
				break;
			case GLFW_KEY_M:
				if (action != GLFW_RELEASE) {
					terrain->updatePower(false);
				}
				break;
		}
	}


	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
	{
		// cTheta += (float) deltaX;
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			mouseDown = true;
			glfwGetCursorPos(window, &posX, &posY);
			Moving = true;
			prevX = posX;
			prevY = posY;
		}

		if (action == GLFW_RELEASE)
		{
			Moving = false;
			mouseDown = false;
			prevX = 0;
			prevY = 0;
		}
	}

	void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if(mouseDown == true)
		{
			phi -=(float)(prevY - ypos);
			prevY = ypos;
			
			if(phi > 80)
			{
				phi = 80;
			}
			else if(phi < -80)
			{
				phi = -80;
			}
			
			theta +=(float) (prevX - xpos);
			prevX = xpos;

		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	// Code to load in textures
	void initTex()
	{
		skyTexture = make_shared<Texture>();
		skyTexture->setFilename(resourceDir + "/night.jpg");
		skyTexture->init();
		skyTexture->setUnit(0);
		skyTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		sunTexture = make_shared<Texture>();
		sunTexture->setFilename(resourceDir + "/crazy.jpg");
		sunTexture->init();
		sunTexture->setUnit(1);
		sunTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		particleTexture = make_shared<Texture>();
		particleTexture->setFilename(resourceDir + "/alpha.bmp");
		particleTexture->init();
		particleTexture->setUnit(4);
		particleTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);	

	}

	void shapeSetUp()
	{
		// Initialize the GLSL program.
		shapeProg = make_shared<Program>();
		shapeProg->setVerbose(true);
		shapeProg->setShaderNames(
				resourceDir + "/phong_vert.glsl",
				resourceDir + "/phong_frag.glsl");
		if (! shapeProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		shapeProg->addUniform("P");
		shapeProg->addUniform("V");
		shapeProg->addUniform("M");
		shapeProg->addUniform("MatAmb");
		shapeProg->addUniform("MatDif");
		shapeProg->addUniform("lightPos");
	    shapeProg->addUniform("MatSpec");
	    shapeProg->addUniform("shine");
		shapeProg->addAttribute("vertPos");
		shapeProg->addAttribute("vertNor");
	}

	void particleSetUp()
	{
		particleProg = make_shared<Program>();
		particleProg->setVerbose(true);
		particleProg->setShaderNames(
				resourceDir + "/particle_vert.glsl",
				resourceDir + "/particle_frag.glsl");
		if (! particleProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		particleProg->addUniform("P");
		particleProg->addUniform("V");
		particleProg->addUniform("M");
		particleProg->addUniform("alphaTexture");
		particleProg->addAttribute("vertPos");
	}

	void skySetUp()
	{
		skyProg = make_shared<Program>();
		skyProg->setVerbose(true);
		skyProg->setShaderNames(
				resourceDir + "/sky_tex_vert.glsl",
				resourceDir + "/sky_tex_frag.glsl");
		if (! skyProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		skyProg->addUniform("P");
		skyProg->addUniform("V");
		skyProg->addUniform("M");
		skyProg->addUniform("Texture0");
		skyProg->addUniform("Texture1");
		skyProg->addUniform("lightPos");
		skyProg->addAttribute("vertPos");
		skyProg->addAttribute("vertNor");
		skyProg->addAttribute("vertTex");
	}

	void init()
	{
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		glClearColor(.12f, .34f, .56f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		
		skySetUp();
		shapeSetUp();
		particleSetUp();
		terrain->initTerrain();
		//terrain->generateGrid();
	}

	void initParticles()
	{
		int n = numP;

		for (int i = 0; i < n; ++ i)
		{
			auto particle = make_shared<Particle>();
			particles.push_back(particle);
			particle->load();
		}
	}


	void initSceneCollectibles()
    {
        uploadMultipleShapes("/fruits/strawberries.obj", 0);
        for(int i = 0; i < 6; i++)
        {
            bool found_spot = false;
            Strawberry *berry;
            while (!found_spot)
            {
                berry = new Strawberry();
                berry->initObject(strawMin, strawMax, i, GameObject::strawberry);
                BoundingBox *otherBB = berry->getBB();
                for (int j = 0; j < objects.size(); j++)
                {
                    berry = new Strawberry();
                    berry->initObject(strawMin, strawMax, i, GameObject::strawberry);
                    BoundingBox *otherBB = berry->getBB();
                    if ((objects[j]->isCollided(otherBB)))
                    {
                        delete berry;
                        delete otherBB;
                    }
                }
                found_spot = true;
                delete otherBB;
            }

            objects.push_back(berry);
        }
    }

    void initSceneObjects()
    {
		uploadMultipleShapes("/crystal1.obj", 1);
		uploadMultipleShapes("/crystal2.obj", 2);
		uploadMultipleShapes("/crystal3.obj", 3);
		numCrystals = rand() % 100;

        for(int i = 0; i < numCrystals; i++)
        {
//            bool found_spot = false;
            Crystal *crystal;

//            while (!found_spot)
//            {
                crystal = new Crystal();
                GameObject::objType crystal_type = selectRandomCrystal();
                crystal = new Crystal();

                if (crystal_type == GameObject::crystal1)
                {
                    crystal->initObject(cryst1min, cryst1max, i % 5, crystal_type);
                }
                else if (crystal_type == GameObject::crystal2)
                {
                    crystal->initObject(cryst2min, cryst2max, i % 5, crystal_type);
                }
                else if (crystal_type == GameObject::crystal3)
                {
                    crystal->initObject(cryst3min, cryst3max, i % 5, crystal_type);
                }

//                BoundingBox *otherBB = crystal->getBB();
//                for (int j = 0; j < objects.size(); j++)
//                {
//                    BoundingBox *otherBB = crystal->getBB();
//                    if ((objects[j]->isCollided(otherBB))) {
//                        delete crystal;
//                        delete otherBB;
//                    }
//                }
//                found_spot = true;
//                delete otherBB;

//            }
            objects.push_back(crystal);
        }


    }

    GameObject::objType selectRandomCrystal()
	{
		int random = rand() % 2;

		if(random == 0)
		{
			return GameObject::crystal1;
		}
		else if (random == 1)
		{
			return GameObject::crystal2;
		}
		else
		{
			return GameObject::crystal3;
		}

	}

	void initGeom()
	{

		// for ground
		//initQuad();
		initSceneCollectibles();
        initSceneObjects();

		// creation for particles
		CHECKED_GL_CALL(glGenVertexArrays(1, &ParticleVertexArrayID));
		CHECKED_GL_CALL(glBindVertexArray(ParticleVertexArrayID));

		CHECKED_GL_CALL(glGenBuffers(1, &particlePointsBuffer));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, particlePointsBuffer));

		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW));
		CHECKED_GL_CALL(glGenBuffers(1, &particleColorBuffer));
		
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW));
		// creation for particles

        sphereShape = make_shared<Shape>();
        sphereShape->loadMesh(resourceDir + "/sphere.obj");
        sphereShape->resize();
        sphereShape->init();
		terrain->generateGrid();

	}

    void uploadMultipleShapes(string objDir, int switchNum)
    {

        vector<tinyobj::shape_t> TOshapes;
        vector<tinyobj::material_t> objMaterials;
        string errStr;
        bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                                   ("../resources" + objDir).c_str());

        if (!rc)
        {
            cerr << errStr << endl;
        }
        else
        {
            vec3 Gmin, Gmax;
            Gmin = vec3(std::numeric_limits<float>::max());
            Gmax = vec3(-std::numeric_limits<float>::max());
            for (size_t i = 0; i < TOshapes.size(); i++)
            {
                shared_ptr<Shape> s =  make_shared<Shape>();
                s->createShape(TOshapes[i]);
                s->measure();

                if(s->min.x < Gmin.x)
                {
                    Gmin.x = s->min.x;
                }

                if(s->max.x > Gmax.x)
                {
                    Gmax.x = s->max.x;
                }

                if(s->min.y < Gmin.y)
                {
                    Gmin.y = s->min.y;
                }

                if(s->max.y > Gmax.y)
                {
                    Gmax.y = s->max.y;
                }

                if(s->min.z < Gmin.z)
                {
                    Gmin.z = s->min.z;
                }

                if(s->max.z > Gmax.z)
                {
                    Gmax.z = s->max.z;
                }

                s->init();


                switch (switchNum)
				{
					case 0:
						strawMin = Gmin;
						strawMax = Gmax;
						strawberryShapes.push_back(s);
						break;
					case 1:
						cryst1max = Gmax;
						cryst1min = Gmin;
						crystal1Shapes.push_back(s);
					case 2:
						cryst2max = Gmax;
						cryst2min = Gmin;
						crystal2Shapes.push_back(s);
					case 3:
						cryst3max = Gmax;
						cryst3min = Gmin;
						crystal3Shapes.push_back(s);
				}
            }
        }
    }

	void updateGeom(float dt)
	{
		glm::vec3 pos;
		glm::vec4 col;

		for (int i = 0; i < numP; i++)
		{
			pos = particles[i]->getPosition();
			col = particles[i]->getColor();
			points[i * 3 + 0] = pos.x;
			points[i * 3 + 1] = pos.y;
			points[i * 3 + 2] = pos.z;
			pointColors[i * 4 + 0] = col.r + col.a / 10.f;
			pointColors[i * 4 + 1] = col.g + col.g / 10.f;
			pointColors[i * 4 + 2] = col.b + col.b / 10.f;
			pointColors[i * 4 + 3] = col.a;
		}


		// update the GPU data
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, particlePointsBuffer));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(points), NULL, GL_STREAM_DRAW));
		CHECKED_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * numP * 3, points));

		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(pointColors), NULL, GL_STREAM_DRAW));
		CHECKED_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * numP * 4, pointColors));

		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	void updateParticles()
	{
		// update the particles
		for (auto particle : particles)
		{
			particle->update(t, h, g, keyToggles);
		}
		t += h;

		// Sort the particles by Z
		auto temp = make_shared<MatrixStack>();
		temp->rotate(y, vec3(0, 1, 0));

		ParticleSorter sorter;
		sorter.C = temp->topMatrix();
		std::sort(particles.begin(), particles.end(), sorter);
	}

	bool checkForEdge(vec3 hold)
	{
        for(int i = 0; i < objects.size(); i++)
        {
            vec3 pos = objects[i]->currentPos;
            if ((pos.x < -GROUND_SIZE) || (pos.x > GROUND_SIZE) ||
                (pos.z < -GROUND_SIZE) || (pos.z > GROUND_SIZE))
            {

            }
        }

	    float DistPosX = hold.x - GROUND_SIZE;
		float DistNegX = hold.x + GROUND_SIZE;
		float DistPosZ = hold.z - GROUND_SIZE;
		float DistNegZ = hold.z + GROUND_SIZE;


		if(abs(DistPosZ) <= 1.f || abs(DistPosX) <= 1.f || abs(DistNegX) <= 1.f || abs(DistNegZ) <= 1.f)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool checkForObject(vec3 hold)
	{
        for(int i = 0; i < objects.size(); i++)
        {
            if (objects[i]->isCollided(hold))
            {
                if (dynamic_cast<Strawberry*>(objects[i]) != nullptr)
                {
                    auto s = dynamic_cast<Strawberry*>(objects[i]);
                    int color = s->collect();
                    if(color == 0)
					{
                    	red = true;
					}
                    else if(color == 1)
					{
                    	orange = true;
					}
                    else if(color == 2)
					{
                    	yellow = true;
					}
					else if(color == 3)
					{
						green = true;
					}
					else if(color == 4)
					{
						blue = true;
					}
					else if(color == 5)
					{
						violet = true;
					}

					objects.erase(objects.begin()+i);

                }
                else
                {
                    return false;
                }
            }



        }

        return true;
	}


	void drawParticles(MatrixStack* View, float aspect)
	{
		particleProg->bind();
		updateParticles();


		auto Model = make_shared<MatrixStack>();
		Model->pushMatrix();
			Model->loadIdentity();

		auto Projection = make_shared<MatrixStack>();
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, 150.0f);

		particleTexture->bind(particleProg->getUniform("alphaTexture"));
		CHECKED_GL_CALL(glUniformMatrix4fv(particleProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix())));
		CHECKED_GL_CALL(glUniformMatrix4fv(particleProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix())));
		CHECKED_GL_CALL(glUniformMatrix4fv(particleProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix())));

		CHECKED_GL_CALL(glEnableVertexAttribArray(0));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, particlePointsBuffer));
		CHECKED_GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0));

		CHECKED_GL_CALL(glEnableVertexAttribArray(1));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, particleColorBuffer));
		CHECKED_GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0));

		CHECKED_GL_CALL(glVertexAttribDivisor(0, 1));
		CHECKED_GL_CALL(glVertexAttribDivisor(1, 1));
		CHECKED_GL_CALL(glDrawArraysInstanced(GL_POINTS, 0, 1, numP));

		CHECKED_GL_CALL(glVertexAttribDivisor(0, 0));
		CHECKED_GL_CALL(glVertexAttribDivisor(1, 0));
		CHECKED_GL_CALL(glDisableVertexAttribArray(0));
		CHECKED_GL_CALL(glDisableVertexAttribArray(1));
		particleTexture->unbind();

		Model->popMatrix();
		particleProg->unbind();
	}

	void drawSky(MatrixStack* View, MatrixStack* Projection)
	{
		auto Model = make_shared<MatrixStack>();
		skyProg->bind();

		mat4 newView = View->topMatrix();

		newView[3][0] = 0.0;
		newView[3][1] = 0.0;
		newView[3][2] = 0.0;

		glUniformMatrix4fv(skyProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(skyProg->getUniform("V"), 1, GL_FALSE, value_ptr(newView));
		glUniform3f(skyProg->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
		Model->pushMatrix();
			Model->loadIdentity();
				Model->pushMatrix();
				Model->rotate(cos(glfwGetTime()/10), vec3(0,1,0));
				Model->scale(vec3(100, 100.f, 100));
				glUniformMatrix4fv(skyProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()) );
				skyTexture->bind(skyProg->getUniform("Texture0"));
				sunTexture->bind(skyProg->getUniform("Texture1"));
                sphereShape->draw(skyProg);
				Model->popMatrix();	
				skyTexture->unbind();
				sunTexture->unbind();


		Model->popMatrix();
		skyProg->unbind();
	}

	void drawScene(MatrixStack* View, MatrixStack* Projection)
	{

		auto Model = make_shared<MatrixStack>();
		shapeProg->bind();

		glUniformMatrix4fv(shapeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(shapeProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(shapeProg->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);

		Model->pushMatrix();
		Model->loadIdentity();

		for(int i = 0; i < objects.size(); i++)
		{
            MatrixStack *modelptr = Model.get();

            if(objects[i]->type == GameObject::strawberry)
			{
				objects[i]->drawObject(modelptr, strawberryShapes, shapeProg);
			}
			else if(objects[i]->type == GameObject::crystal1)
			{
                CHECKED_GL_CALL(glEnable(GL_BLEND));
                CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
				objects[i]->drawObject(modelptr, crystal1Shapes, shapeProg);
                CHECKED_GL_CALL(glDisable(GL_BLEND));
			}
			else if(objects[i]->type == GameObject::crystal2)
			{
                CHECKED_GL_CALL(glEnable(GL_BLEND));
                glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);
				objects[i]->drawObject(modelptr, crystal2Shapes, shapeProg);
                CHECKED_GL_CALL(glDisable(GL_BLEND));
			}
			else if(objects[i]->type == GameObject::crystal3)
			{
                CHECKED_GL_CALL(glEnable(GL_BLEND));
                CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
				objects[i]->drawObject(modelptr, crystal3Shapes, shapeProg);
                CHECKED_GL_CALL(glDisable(GL_BLEND));
			}

		}
			

		Model->popMatrix();
		shapeProg->unbind();
	}

    void render(float deltaTime)
    {

        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        updateGeom(deltaTime);

        lightPos.x = cos(glfwGetTime()/40) * 500.f;
        lightPos.z = sin(glfwGetTime()/40) * 500.f;

        float aspect = width/(float)height;

        x = cos(radians(phi))*cos(radians(theta));
        y = sin(radians(phi));
        z = cos(radians(phi))*sin(radians(theta));

        vec3 forward = vec3(x, y, z);
        vec3 up = vec3(0,1,0);
        vec3 sides = cross(forward, up);

        float actualSpeed = MOVEMENT_SPEED;
        if(sprint)
        {
            actualSpeed *= 3;
        }

        vec3 holdCameraPos = cameraPos;

        if(moveForward)
        {
            holdCameraPos = cameraPos + (forward * actualSpeed);
        }
        if(moveBackward)
        {
            holdCameraPos = cameraPos - (forward * actualSpeed);
        }
        if(moveLeft)
        {
            holdCameraPos = cameraPos - (sides * actualSpeed);
        }
        if(moveRight)
        {
            holdCameraPos = cameraPos + (sides * actualSpeed);
        }

        bool go = checkForEdge(holdCameraPos);
        go = checkForObject(holdCameraPos);

        if(go)
        {
            cameraPos = holdCameraPos;
        }

        auto ViewUser = make_shared<MatrixStack>();
        ViewUser->pushMatrix();
        ViewUser->loadIdentity();
        ViewUser->pushMatrix();
        ViewUser->lookAt(vec3(cameraPos.x, 1.0, cameraPos.z), forward + vec3(cameraPos.x, 1.0, cameraPos.z), up);
        MatrixStack *userViewPtr = ViewUser.get();



        auto Projection = make_shared<MatrixStack>();
        Projection->pushMatrix();
        Projection->perspective(45.0f, aspect, 0.01f, GROUND_SIZE);
        MatrixStack *projectionPtr = Projection.get();

        CHECKED_GL_CALL(glDisable(GL_DEPTH_TEST));
        CHECKED_GL_CALL(glDisable(GL_BLEND));
        drawSky(userViewPtr, projectionPtr);

        CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
        drawScene(userViewPtr, projectionPtr);

        CHECKED_GL_CALL(glDisable(GL_BLEND));

		CHECKED_GL_CALL(glEnable(GL_BLEND));
		CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		CHECKED_GL_CALL(glPointSize(25.0f));
		drawParticles(userViewPtr, aspect);

		auto Model = make_shared<MatrixStack>();
		Model->pushMatrix();
		Model->loadIdentity();

		Model->pushMatrix();
		Model->translate(vec3(-5, 0, -8));

		terrain->render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), cameraPos);
		Model->popMatrix();


		Projection->popMatrix();
		ViewUser->popMatrix();
		ViewUser->popMatrix();
    }

};

int main(int argc, char **argv)
{
	Application *application = new Application();

	WindowManager *windowManager = new WindowManager();
	windowManager->init(1024, 1024);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	application->init();
	application->initTex();
	application->initParticles();
	application->initGeom();
    auto lastTime = chrono::high_resolution_clock::now();

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
            // save current time for next frame
            auto nextLastTime = chrono::high_resolution_clock::now();

            // get time since last frame
            float deltaTime =
                    chrono::duration_cast<std::chrono::microseconds>(
                            chrono::high_resolution_clock::now() - lastTime)
                            .count();
			// Render scene.
			application->render(deltaTime);

			// Swap front and back buffers.
			glfwSwapBuffers(windowManager->getHandle());

			// Poll for and process events.
			glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
