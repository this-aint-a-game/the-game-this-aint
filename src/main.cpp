/*
Alexa
Caroline
Doug

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
#include "Water.h"
#include "Crystal.h"
//#include "Player.h"
#include "Camera.h"
#include "Lighting.h"
#include "Sky.h"
#include "ColorCollectGameplay.h"


#define PI 3.1415
#define MOVEMENT_SPEED 0.2f
#define RENDER_SPEED 0.5f

using namespace std;
using namespace glm;

bool debug = true;

class Application : public EventCallbacks
{

public:

    ColorCollectGameplay * gameplay = new ColorCollectGameplay();
    Player player = Player();
	Camera camera = Camera();
	Terrain terrain = Terrain(gameplay);
	Water water = Water(gameplay);
	Sky sky = Sky();

	WindowManager * windowManager = nullptr;
	int width, height;

	bool releaseMouse = false;

	std::string resourceDir = "../resources";
	vector<GameObject*> objects;

	// programs
	shared_ptr<Program> shapeProg;
	shared_ptr<Program> particleProg;

	Lighting* lighting = new Lighting();

	shared_ptr<Texture> particleTexture;

	// Shape to be used (from obj file)
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
	int numP = 20;
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

	bool sprint = false;
	bool Moving = false;

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


	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{

		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, GL_TRUE);
		}

		if (key == GLFW_KEY_G && action == GLFW_PRESS)
        {
		    if (debug)
            {
		        debug = false;
            } else {
		        debug = true;
		    }
        }

		if (debug) {
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
            }
        }
		else {
            if (key == GLFW_KEY_W && action == GLFW_PRESS) {
                player.w = 1;
            }
            if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
                player.w = 0;
            }
            if (key == GLFW_KEY_S && action == GLFW_PRESS) {
                player.s = 1;
            }
            if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
                player.s = 0;
            }
            if (key == GLFW_KEY_A && action == GLFW_PRESS) {
                player.a = 1;
            }
            if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
                player.a = 0;
            }
            if (key == GLFW_KEY_D && action == GLFW_PRESS) {
                player.d = 1;
            }
            if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
                player.d = 0;
            }            
			if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
                camera.zoomIn = 1;
            }
            if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
                camera.zoomIn = 0;
            }
            if (key == GLFW_KEY_E && action == GLFW_PRESS) {
                camera.zoomOut = 1;
            }
            if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
                camera.zoomOut = 0;
            }
        }
		if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
			glfwSetInputMode(windowManager->getHandle(),
							 GLFW_CURSOR,
							 GLFW_CURSOR_NORMAL);
			releaseMouse = true;
		}
		if (key == GLFW_KEY_BACKSPACE && action == GLFW_RELEASE) {
			glfwSetInputMode(windowManager->getHandle(),
							 GLFW_CURSOR,
							 GLFW_CURSOR_DISABLED);
			releaseMouse = false;
		}
	}

	void scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
    {

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
		sky.initTex();

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
	    shapeProg->addUniform("MatSpec");
	    shapeProg->addUniform("shine");
		shapeProg->addUniform("view");
		shapeProg->addAttribute("vertPos");
		shapeProg->addAttribute("vertNor");
        shapeProg->addUniform("lighting");
        shapeProg->addUniform("numberLights");
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

	void init()
	{
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		glClearColor(.12f, .34f, .56f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		
		sky.skySetUp();
		shapeSetUp();
		particleSetUp();
		terrain.initTerrain();
		water.initWater();
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

        // For now, hard code purple prize close to start
        Strawberry * first = new Strawberry(strawMin, strawMax, 5, GameObject::strawberry, gameplay);
		first->setPosition(-8, 20);
		objects.push_back(first);
        gameplay->setPos(first->color, first->currentPos);

		// For now, hard code blue prize close to start
        Strawberry * second = new Strawberry(strawMin, strawMax, 4, GameObject::strawberry, gameplay);
        second->setPosition(8, 8);
        objects.push_back(second);
        gameplay->setPos(second->color, second->currentPos);


        for(int i = 0; i < 4; i++)
        {
            bool found_spot = false;
            Strawberry *berry;
            while (!found_spot)
            {
                berry = new Strawberry(strawMin, strawMax, i, GameObject::strawberry, gameplay);
                BoundingBox *otherBB = berry->getBB();
                for (int j = 0; j < objects.size(); j++)
                {
                    berry = new Strawberry(strawMin, strawMax, i, GameObject::strawberry, gameplay);
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
            gameplay->setPos(berry->color, berry->currentPos);
        }
    }

    void initSceneObjects()
    {
		uploadMultipleShapes("/crystal1.obj", 1);
//		uploadMultipleShapes("/crystal2.obj", 2);
//		uploadMultipleShapes("/crystal3.obj", 3);
		numCrystals = clamp(rand() % 100, 5, 15);

        for(int i = 0; i < numCrystals; i++)
        {
//            bool found_spot = false;
            Crystal *crystal;

//            while (!found_spot)
//            {

//                GameObject::objType crystal_type = selectRandomCrystal();
                GameObject::objType crystal_type = GameObject::crystal1;

                //if (crystal_type == GameObject::crystal1)
                //{
					crystal = new Crystal(cryst1min, cryst1max, i % 6, crystal_type, gameplay);
                //}
//                else if (crystal_type == GameObject::crystal2)
//                {
//                    crystal->initObject(cryst2min, cryst2max, i % 5, crystal_type);
//                }
//                else if (crystal_type == GameObject::crystal3)
//                {
//                    crystal->initObject(cryst3min, cryst3max, i % 5, crystal_type);
//                }

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
		int random = rand() % 3;

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
        player.initPlayer(gameplay);
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
			particle->update(t, h, g, keyToggles, player.position, gameplay);
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

	void drawParticles(MatrixStack* View, float aspect)
	{
		particleProg->bind();
		updateParticles();

		auto Model = make_shared<MatrixStack>();
		Model->pushMatrix();
			Model->loadIdentity();

        auto Projection = make_shared<MatrixStack>();
        Projection->pushMatrix();
        Projection->perspective(45.0f, aspect, 0.01f, GROUND_SIZE);

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

	void drawScene(MatrixStack* View, MatrixStack* Projection)
	{

		auto Model = make_shared<MatrixStack>();
		shapeProg->bind();

		glUniformMatrix4fv(shapeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(shapeProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
        glUniform1f(shapeProg->getUniform("numberLights"), lighting->numberLights);
		lighting->bind(shapeProg->getUniform("lighting"));

		Model->pushMatrix();
		Model->loadIdentity();

		for(int i = 0; i < objects.size(); i++)
		{
            MatrixStack *modelptr = Model.get();

            if(objects[i]->type == GameObject::strawberry)
			{
				objects[i]->drawObject(modelptr, strawberryShapes, shapeProg, camera.getPosition());
			}
			else if(objects[i]->type == GameObject::crystal1)
			{
                CHECKED_GL_CALL(glEnable(GL_BLEND));
                glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);
				objects[i]->drawObject(modelptr, crystal1Shapes, shapeProg, camera.getPosition());
                CHECKED_GL_CALL(glDisable(GL_BLEND));
			}
//			else if(objects[i]->type == GameObject::crystal2)
//			{
//                CHECKED_GL_CALL(glEnable(GL_BLEND));
//                glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);
//				objects[i]->drawObject(modelptr, crystal2Shapes, shapeProg);
//                CHECKED_GL_CALL(glDisable(GL_BLEND));
//			}
//			else if(objects[i]->type == GameObject::crystal3)
//            {
//                CHECKED_GL_CALL(glEnable(GL_BLEND));
//                CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
//				objects[i]->drawObject(modelptr, crystal3Shapes, shapeProg);
//                CHECKED_GL_CALL(glDisable(GL_BLEND));
//			}

		}

		Model->popMatrix();
		lighting->unbind();
		shapeProg->unbind();

	}

    void render(float deltaTime)
    {

        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// need mouse position in order to use camera
		double mousex = width / 4.0;
		double mousey = height / 4.0;    
		float aspect = width/(float)height;

		if(!releaseMouse)
			glfwGetCursorPos(windowManager->getHandle(), &mousex, &mousey);

        updateGeom(deltaTime);

        if (!player.checkForCollisions(objects))
        {
			player.updateView(deltaTime * 0.000001f, mousex, mousey, width,
							  height);
		}
        //mat4 playerM = player.update(deltaTime* 0.000001f, mousex, mousey, width, height);
		//playerM *= scale(mat4(1), vec3(0.2,0.2,0.2));

        lightPos.x = cos(glfwGetTime()/100) * 500.f;
        lightPos.z = sin(glfwGetTime()/100) * 500.f;

        auto ViewUser = make_shared<MatrixStack>();
        ViewUser->pushMatrix();
        ViewUser->loadIdentity();
        ViewUser->pushMatrix();

        if (!debug) {
            ViewUser->multMatrix(
                    camera.update(player.position, deltaTime * 0.000001f,
                                  mousex, mousey, width, height));

            // reset mouse position to center of screen after finding difference.

            if (!releaseMouse)
                glfwSetCursorPos(windowManager->getHandle(), width / 4.0,
                                 height / 4.0);
        } else {
            x = cos(radians(phi)) * cos(radians(theta));
            y = sin(radians(phi));
            z = cos(radians(phi)) * sin(radians(theta));

            vec3 forward = vec3(x, y, z);
            vec3 up = vec3(0, 1, 0);
            vec3 sides = cross(forward, up);

            float actualSpeed = MOVEMENT_SPEED;
            if (sprint) {
                actualSpeed *= 3;
            }

            vec3 holdCameraPos = cameraPos;

            if (moveForward) {
                holdCameraPos = cameraPos + (forward * actualSpeed);
            }
            if (moveBackward) {
                holdCameraPos = cameraPos - (forward * actualSpeed);
            }
            if (moveLeft) {
                holdCameraPos = cameraPos - (sides * actualSpeed);
            }
            if (moveRight) {
                holdCameraPos = cameraPos + (sides * actualSpeed);
            }

            /*
            bool go = checkForEdge(holdCameraPos);
            go = checkForObject(holdCameraPos);

            if (go) {
                cameraPos = holdCameraPos;
            }
            */

			cameraPos = holdCameraPos;
            ViewUser->lookAt(vec3(cameraPos.x, 1.0, cameraPos.z),
                             forward + vec3(cameraPos.x, 1.0, cameraPos.z), up);
        }

        MatrixStack *userViewPtr = ViewUser.get();

        auto Projection = make_shared<MatrixStack>();
        Projection->pushMatrix();
        Projection->perspective(45.0f, aspect, 0.01f, 150.f);
        MatrixStack *projectionPtr = Projection.get();

        CHECKED_GL_CALL(glDisable(GL_DEPTH_TEST));
        CHECKED_GL_CALL(glDisable(GL_BLEND));
        sky.drawSky(userViewPtr, projectionPtr, lightPos, glfwGetTime()/1000);


        CHECKED_GL_CALL(glDisable(GL_BLEND));
        CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
        drawScene(userViewPtr, projectionPtr);


		auto Model = make_shared<MatrixStack>();
		Model->pushMatrix();
		Model->loadIdentity();
		Model->pushMatrix();


        if (!debug)
        {
            terrain.render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), camera.getPosition(), lighting);
        }
        else
        {
            terrain.render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), cameraPos, lighting);
        }

        CHECKED_GL_CALL(glEnable(GL_BLEND));
        CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		water.render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), cameraPos);
		Model->popMatrix();


        CHECKED_GL_CALL(glDisable(GL_BLEND));
		player.drawPlayer(userViewPtr, projectionPtr, camera.getPosition(), lighting);


        CHECKED_GL_CALL(glEnable(GL_BLEND));
        CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
        CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        CHECKED_GL_CALL(glPointSize(25.0f));
        drawParticles(userViewPtr, aspect);

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

    glfwSetInputMode(windowManager->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    application->initGeom();
	application->lighting->init();
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

			// reset lastTime so that we can calculate the deltaTime
			// on the next frame
			lastTime = nextLastTime;

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
