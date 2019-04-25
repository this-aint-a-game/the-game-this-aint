/*
Alexa Drenick
Caroline Cullen 
lab 1
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

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>

#define PI 3.1415
#define MOVEMENT_SPEED 0.2f
#define GROUND_SIZE 120
#define RENDER_SPEED 0.5f

using namespace std;
using namespace glm;

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;
	int width, height;
	std::string resourceDir = "../resources";
	int score;
	vector<GameObject*> objects;

	// programs
	shared_ptr<Program> shapeProg;
	//shared_ptr<Program> groundProg;
	shared_ptr<Program> particleProg;
    shared_ptr<Program> deadTreesProg;
	shared_ptr<Program> skyProg;

	Terrain *terrain = new Terrain();

	// collection 
	bool colStrawberries = false;


	// textures
	shared_ptr<Texture> skyTexture;
	shared_ptr<Texture> sunTexture;
	shared_ptr<Texture> groundTexture;
	shared_ptr<Texture> particleTexture;
	shared_ptr<Texture> deadTreeTexture;

	//ground info
	GLuint GrndBuffObj, GrndNorBuffObj, GrndTexBuffObj, GIndxBuffObj;
	int gGiboLen;

	//transforms for the world
	vec3 worldTrans = vec3(0);
	float worldScale = 1.0;

	// Shape to be used (from obj file)
	shared_ptr<Shape> sphereShape;
	shared_ptr<Shape> deadTree;

    vector<shared_ptr<Shape>> strawberryShapes;
    int numOfStraw = 0;

    glm::vec3 strawMin = glm::vec3(0);
    glm::vec3 strawMax = glm::vec3(0);

	// Contains vertex information for OpenGL
	GLuint GroundVertexArrayID;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID;

	//geometry for texture render
	GLuint quad_VertexArrayID;
	GLuint quad_vertexbuffer;

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

	int numDeadTrees;
	vector<GLfloat> deadPositions;
	vector<GLfloat> deadScales;
	vector<GLfloat> deadRotations;

	double time_increment = 0;
	double lastTime = glfwGetTime();
	double deltaTime = 0;

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
		skyTexture->setFilename(resourceDir + "/nightSky.jpg");
		skyTexture->init();
		skyTexture->setUnit(0);
		skyTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		sunTexture = make_shared<Texture>();
		sunTexture->setFilename(resourceDir + "/crazy.jpg");
		sunTexture->init();
		sunTexture->setUnit(1);
		sunTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	 	/*
		groundTexture = make_shared<Texture>();
		groundTexture->setFilename(resourceDir + "/ground.jpg");
		groundTexture->init();
		groundTexture->setUnit(2);
		groundTexture->setWrapModes(GL_REPEAT, GL_REPEAT);
	 	 */

		deadTreeTexture = make_shared<Texture>();
		deadTreeTexture->setFilename(resourceDir + "/nightSky.jpg");
		deadTreeTexture->init();
		deadTreeTexture->setUnit(3);
		deadTreeTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

		particleTexture = make_shared<Texture>();
		particleTexture->setFilename(resourceDir + "/alpha.bmp");
		particleTexture->init();
		particleTexture->setUnit(4);
		particleTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);	

	}

	void deadTreeSetUp()
	{
		//initialize the textures we might use
		deadTreesProg = make_shared<Program>();
		deadTreesProg->setVerbose(true);
		deadTreesProg->setShaderNames(
				resourceDir + "/tex_vert.glsl",
				resourceDir + "/tex_frag.glsl");
		if (! deadTreesProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		deadTreesProg->addUniform("P");
		deadTreesProg->addUniform("V");
		deadTreesProg->addUniform("M");
		deadTreesProg->addUniform("Texture0");
		deadTreesProg->addUniform("texNum");
		deadTreesProg->addAttribute("vertPos");
		deadTreesProg->addAttribute("vertNor");
		deadTreesProg->addAttribute("vertTex");
		deadTreesProg->addUniform("lightPos");
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
		//groundSetUp();
		deadTreeSetUp();
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

	void initDeadTrees()
	{
		numDeadTrees = randGen(50.f, 150.f);
		
		for(int i = 0; i < numDeadTrees; i++)
		{
			deadPositions.push_back(randGen(-GROUND_SIZE, GROUND_SIZE));
			deadPositions.push_back(randGen(-GROUND_SIZE, GROUND_SIZE));
			deadScales.push_back(randGen(6.0f, 13.0f));
			deadRotations.push_back(randGen(0.0f, 180.0f));
		}
	}


	void initSceneCollectibles()
    {
        for(int i = 0; i < 6; i++)
        {
            bool found_spot = false;
            Strawberry *berry;
            while (!found_spot)
            {
                berry = new Strawberry();
                berry->initObject(strawMin, strawMax);
                BoundingBox *otherBB = berry->getBB();
                for (int i = 0; i < objects.size(); i++)
                {
                    berry = new Strawberry();
                    berry->initObject(strawMin, strawMax);
                    BoundingBox *otherBB = berry->getBB();
                    if ((objects[i]->isCollided(otherBB)))
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


	void initGeom()
	{
		deadTree = make_shared<Shape>();
		deadTree->loadMesh(resourceDir + "/deadTree.obj");
		deadTree->resize();
		deadTree->init();

		uploadMultipleShapes("/fruits/strawberries.obj", 0);

		// for ground
		//initQuad();
		initSceneCollectibles();

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

	/*
	void initQuad()
	{
		float g_groundSize = GROUND_SIZE;
		float g_groundY = -1.5;

		// A x-z plane at y = g_groundY of dim[-g_groundSize, g_groundSize]^2
		float GrndPos[] = {
			-g_groundSize, g_groundY, -g_groundSize,
			-g_groundSize, g_groundY,  g_groundSize,
			 g_groundSize, g_groundY,  g_groundSize,
			 g_groundSize, g_groundY, -g_groundSize
		};

		float GrndNorm[] = {
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0,
			0, 1, 0
		};

		float GrndTex[] = {
			0, 0, // back
			0, 1,
			1, 1,
			1, 0
		};

		unsigned short idx[] = {0, 1, 2, 0, 2, 3};

		GLuint GroundVertexArrayID;
		//generate the VAO
		glGenVertexArrays(1, &GroundVertexArrayID);
		glBindVertexArray(GroundVertexArrayID);

		gGiboLen = 6;
		glGenBuffers(1, &GrndBuffObj);
		glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GrndPos), GrndPos, GL_STATIC_DRAW);

		glGenBuffers(1, &GrndNorBuffObj);
		glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GrndNorm), GrndNorm, GL_STATIC_DRAW);

		glGenBuffers(1, &GrndTexBuffObj);
		glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

		glGenBuffers(1, &GIndxBuffObj);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
	}
	 */

	/*
	void renderGround()
	{

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, GrndBuffObj);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, GrndNorBuffObj);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GIndxBuffObj);
		glDrawElements(GL_TRIANGLES, gGiboLen, GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}
	 */

	void render()
	{

		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		glViewport(0, 0, width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		double nowTime = glfwGetTime();
//		deltaTime += (nowTime - lastTime);

		time_increment += (nowTime - lastTime);

		updateGeom(nowTime - lastTime);

		lastTime = nowTime;

		lightPos.x = cos(glfwGetTime()/12) * 500.f;
		lightPos.z = sin(glfwGetTime()/12) * 500.f;

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

		auto Model = make_shared<MatrixStack>();
		Model->pushMatrix();
		Model->loadIdentity();

		checkForFruit();

		auto Projection = make_shared<MatrixStack>();
		Projection->pushMatrix();
		Projection->perspective(45.0f, aspect, 0.01f, GROUND_SIZE);
		MatrixStack *projectionPtr = Projection.get();


		CHECKED_GL_CALL(glDisable(GL_DEPTH_TEST));
		CHECKED_GL_CALL(glDisable(GL_BLEND));
		drawSky(userViewPtr, projectionPtr);

		CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
		drawScene(userViewPtr, projectionPtr);
		//drawGround(userViewPtr, projectionPtr);

		//drawDeadTrees(userViewPtr, projectionPtr);
		
		CHECKED_GL_CALL(glEnable(GL_BLEND));
		CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		CHECKED_GL_CALL(glPointSize(25.0f));
		drawParticles(userViewPtr, aspect);

        Model->pushMatrix();
        Model->translate(vec3(-5, 0, -8));

        terrain->render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), cameraPos);
        Model->popMatrix();


		Projection->popMatrix();
		ViewUser->popMatrix();
		ViewUser->popMatrix();	
	}

	bool checkForEdge(vec3 hold)
	{
        for(int i = 0; i < objects.size(); i++)
        {
            vec3 pos = objects[i]->currentPos;
            if ((pos.x < -GROUND_SIZE) || (pos.x > GROUND_SIZE) ||
                (pos.z < -GROUND_SIZE) || (pos.z > GROUND_SIZE))
            {
                objects[i]->velocity *= -1;
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

	void checkForFruit()
	{
		for(int i = 0; i < objects.size(); i++)
		{
			if((objects[i]->isCollided(cameraPos)) && (objects[i]->collected == false))
			{
			    objects[i]->collected = true;
				score++;
				cout << "Score: " << score << endl;
			}
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

	void drawDeadTrees(MatrixStack* View, MatrixStack* Projection)
	{
		auto Model = make_shared<MatrixStack>();

		deadTreesProg->bind();
		glUniformMatrix4fv(deadTreesProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(deadTreesProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(deadTreesProg->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);

		Model->pushMatrix();
			Model->loadIdentity();
			for (int i = 0; i < numDeadTrees; i+=3)
			{
				/* draw left mesh */
				GLfloat treeS = deadScales[i/3];
				GLfloat treeR = deadRotations[i/3];
				Model->pushMatrix();
				Model->translate(vec3(deadPositions[i], treeS/1.60, deadPositions[i+2]));
				Model->scale(vec3(treeS));
				Model->rotate(treeR, vec3(0, 1, 0));
				glUniformMatrix4fv(deadTreesProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()) );
				deadTreeTexture->bind(deadTreesProg->getUniform("Texture0"));
				glUniform1f(deadTreesProg->getUniform("texNum"), 1);
				deadTree->draw(deadTreesProg);
				Model->popMatrix();
				deadTreeTexture->unbind();
			}
				


		Model->popMatrix();
		deadTreesProg->unbind();
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
		Program *sProgPtr = shapeProg.get();

		shapeProg->bind();
		glUniformMatrix4fv(shapeProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
		glUniformMatrix4fv(shapeProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
		glUniform3f(shapeProg->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);

		Model->pushMatrix();
		Model->loadIdentity();

		for(int i = 0; i < objects.size(); i++)
		{
			// for strawberries
			Model->pushMatrix();
			Model->translate(vec3(objects[i]->currentPos.x, 0.50f, objects[i]->currentPos.z));
			Model->rotate(glfwGetTime()/2, vec3(0,1,0));
			Model->scale(vec3(1.f,1.0f,1.0f));
			for (size_t j = 0; j < strawberryShapes.size(); j++)
			{
			    if(!(objects[i]->collected))
                {
                    if(j == 0)
                    {
                        if(objects[i]->hit_count%3 == 0)
                        {
                            SetMaterial(7, sProgPtr);
                        }
                        else
                        {
                            SetMaterial(objects[i]->hit_count%18, sProgPtr);
                        }


                    }
                    else if (j == 2)
                    {
                        SetMaterial(8, sProgPtr);
                    }
                    else
                    {
                        SetMaterial(6, sProgPtr);
                    }
                }
			    else
                {
                    SetMaterial(6, sProgPtr);
                }
				glUniformMatrix4fv(shapeProg->getUniform("M"), 1, GL_FALSE,value_ptr(Model->topMatrix()) );
				strawberryShapes[j]->draw(shapeProg);
			}
			Model->popMatrix();

		}
			

		Model->popMatrix();
		shapeProg->unbind();
	}

	// TODO this should be in Program
	void SetMaterial(int i, Program *prog)
	{
		switch (i)
		{

	    case 0: // flat grey
		    glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
		    glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
		    glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
		    glUniform1f(prog->getUniform("shine"), 4.0);
		    break;
        case 1: // shiny blue plastic
            glUniform3f(prog->getUniform("MatAmb"), 0.02, 0.04, 0.2);
            glUniform3f(prog->getUniform("MatDif"), 0.0, 0.16, 0.9);
            glUniform3f(prog->getUniform("MatSpec"), 0.14, 0.2, 0.8);
            glUniform1f(prog->getUniform("shine"), 120.0);
            break;
	    case 2: // brass
		    glUniform3f(prog->getUniform("MatAmb"), 0.3294, 0.2235, 0.02745);
		    glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
		    glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
		    glUniform1f(prog->getUniform("shine"), 27.9);
		    break;
        case 3: // pearl
	        glUniform3f(prog->getUniform("MatAmb"), 0.25f, 0.20725f, 0.20725f);
	        glUniform3f(prog->getUniform("MatDif"), 1.0f, 0.829f, 0.829f);
	        glUniform3f(prog->getUniform("MatSpec"), 0.296648f, 0.296648f, 0.296648f);
	        glUniform1f(prog->getUniform("shine"), 11.264f);
	        break;
	    case 4: // copper
	        glUniform3f(prog->getUniform("MatAmb"),  0.19125f, 0.0735f, 0.0225f);
	        glUniform3f(prog->getUniform("MatDif"), 0.7038f, 0.27048f, 0.0828f);
	        glUniform3f(prog->getUniform("MatSpec"), 0.256777f, 0.137622f, 0.086014f);
	        glUniform1f(prog->getUniform("shine"), 12.8f);
	        break;
		case 5: // turqoise
	        glUniform3f(prog->getUniform("MatAmb"),  0.1f, 0.18725f, 0.1745f);
	        glUniform3f(prog->getUniform("MatDif"), 0.396f, 0.74151f, 0.69102f);
	        glUniform3f(prog->getUniform("MatSpec"), 0.297254f, 0.30829f, 0.306678f);
	        glUniform1f(prog->getUniform("shine"), 12.8f);
	        break;
	    case 6: // obisdian
	        glUniform3f(prog->getUniform("MatAmb"),  0.05375f, 0.05f, 0.06625f);
	        glUniform3f(prog->getUniform("MatDif"), 0.18275f, 0.17f, 0.22525f);
	        glUniform3f(prog->getUniform("MatSpec"), 0.332741f, 0.328634f, 0.346435f);
	        glUniform1f(prog->getUniform("shine"), 38.4f);
	        break;
	    case 7: // ruby
	        glUniform3f(prog->getUniform("MatAmb"),  0.1745f, 0.01175f, 0.01175f);
	        glUniform3f(prog->getUniform("MatDif"), 0.61424f, 0.04136f, 0.04136f);
	        glUniform3f(prog->getUniform("MatSpec"), 0.727811f, 0.626959f, 0.626959f);
	        glUniform1f(prog->getUniform("shine"), 76.8f);
	        break;
	    case 8: // emerald
	        glUniform3f(prog->getUniform("MatAmb"),  0.0215f, 0.1745f, 0.0215f);
	        glUniform3f(prog->getUniform("MatDif"), 0.07568f, 0.61424f, 0.07568f);
	        glUniform3f(prog->getUniform("MatSpec"), 0.633f, 0.727811f, 0.633f);
	        glUniform1f(prog->getUniform("shine"), 76.8f);
	        break;
	    case 9: // yellow plastic
	        glUniform3f(prog->getUniform("MatAmb"),  0.05f,0.05f,0.0f);
	        glUniform3f(prog->getUniform("MatDif"), 0.5f,0.5f,0.4f);
	        glUniform3f(prog->getUniform("MatSpec"), 0.7f,0.7f,0.04f);
	        glUniform1f(prog->getUniform("shine"), 10.0f);
	        break;

	    case 10: // blueberries
	        glUniform3f(prog->getUniform("MatAmb"),  0.105882f, 0.058824f, 0.313725f);
	        glUniform3f(prog->getUniform("MatDif"), 0.227451f, 0.270588f, 0.741176f);
	        glUniform3f(prog->getUniform("MatSpec"), 0.233333f, 0.233333f, 0.821569f);
	        glUniform1f(prog->getUniform("shine"), 9.84615f);
	        break;

	    case 11: // lemon
	        glUniform3f(prog->getUniform("MatAmb"), 0.4294, 0.4235, 0.02745);
		    glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
		    glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
		    glUniform1f(prog->getUniform("shine"), 27.9);
	        break;

	    case 12: // orange
	        glUniform3f(prog->getUniform("MatAmb"),  0.49125f, 0.135f, 0.0225f);
	        glUniform3f(prog->getUniform("MatDif"), 0.8038f, 0.37048f, 0.0828f);
	        glUniform3f(prog->getUniform("MatSpec"), 0.556777f, 0.537622f, 0.286014f);
	        glUniform1f(prog->getUniform("shine"), 12.8f);
	        break;

	    case 13: // lime
	        glUniform3f(prog->getUniform("MatAmb"),  0.0215f, 0.1745f, 0.0215f);
	        glUniform3f(prog->getUniform("MatDif"), 0.07568f, 0.61424f, 0.07568f);
	        glUniform3f(prog->getUniform("MatSpec"), 0.633f, 0.727811f, 0.633f);
	        glUniform1f(prog->getUniform("shine"), 76.8f);
	        break;

	    case 15: // banana
	        glUniform3f(prog->getUniform("MatAmb"),  0.05f,0.05f,0.0f);
	        glUniform3f(prog->getUniform("MatDif"), 0.5f,0.5f,0.4f);
	        glUniform3f(prog->getUniform("MatSpec"), 0.7f,0.7f,0.04f);
	        glUniform1f(prog->getUniform("shine"), 35.0f);
	        break;

	    case 16: // pith
	        glUniform3f(prog->getUniform("MatAmb"), 0.7294, 0.7235, 0.2745);
		    glUniform3f(prog->getUniform("MatDif"), 0.7804, 0.5686, 0.11373);
		    glUniform3f(prog->getUniform("MatSpec"), 0.9922, 0.941176, 0.80784);
		    glUniform1f(prog->getUniform("shine"), 27.9);
	        break;

	    case 17: // chrome
	        glUniform3f(prog->getUniform("MatAmb"), 0.25f, 0.25f, 0.25f);
		    glUniform3f(prog->getUniform("MatDif"), 0.4f, 0.4f, 0.4f);
		    glUniform3f(prog->getUniform("MatSpec"), 0.774597f, 0.774597f, 0.774597f);
		    glUniform1f(prog->getUniform("shine"), 76.8f);
	        break;

	    case 18: // black plastic
	        glUniform3f(prog->getUniform("MatAmb"), 0.0f, 0.0f, 0.0f);
		    glUniform3f(prog->getUniform("MatDif"), 0.01f, 0.01f, 0.01f);
		    glUniform3f(prog->getUniform("MatSpec"), 0.50f, 0.50f, 0.50f);
		    glUniform1f(prog->getUniform("shine"), 32.0f);
	        break;

		}
	}

	float randGen(float l, float h)
	{
		float r = rand() / (float) RAND_MAX;
		return (1.0f - r) * l + r * h;
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
	application->initDeadTrees();
	application->initGeom();

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
			// Render scene.
			application->render();

			// Swap front and back buffers.
			glfwSwapBuffers(windowManager->getHandle());

			// Poll for and process events.
			glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
