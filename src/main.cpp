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
#include "WindowManager.h"
#include "GLTextureWriter.h"
#include "Particle.h"
#include "Butterfly.h"
#include "Terrain.h"
#include "Water.h"
#include "Shadow.h"
#include "Camera.h"
#include "Lighting.h"
#include "Sky.h"
#include "ObjectCollection.h"
#include "ParticleCollection.h"
#include "ViewFrustumCulling.h"
#include <irrKlang.h>

#define MOVEMENT_SPEED 0.2f
#define RENDER_SPEED 0.5f

using namespace std;
using namespace glm;

bool debug = false;

class Application : public EventCallbacks
{

public:

    irrklang::ISoundEngine* soundEngine;
    ParticleCollection *pc = new ParticleCollection();
    ObjectCollection *oc = new ObjectCollection();
	Butterfly butterfly = Butterfly();
	Camera camera = Camera();
    Lighting* lighting = new Lighting();
	Shadow shadow = Shadow(lighting);
	Terrain terrain = Terrain(oc->gameplay);
	Water water = Water(oc->gameplay);
	Sky sky = Sky();
	ViewFrustumCulling* vfc = new ViewFrustumCulling();

//    shared_ptr<Program> roosterProg;
//    shared_ptr<Texture> roosterTexture;
//    shared_ptr<Shape> rooster;

	WindowManager * windowManager = nullptr;
	int width, height;

	bool releaseMouse = false;
	bool loadrender = true;
	std::string resourceDir = "../resources";

	float t0_disp = 0.0f;
	float t_disp = 0.0f;
	bool keyToggles[256] = { false };


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
    	        oc->player.w = true;
    	    }
    	    if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
                oc->player.w = false;
    	    }
    	    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
                oc->player.s = true;
    	    }
    	    if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
                oc->player.s = false;
    	    }
    	    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
                oc->player.a = true;
    	    }
    	    if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
                oc->player.a = false;
    	    }
    	    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
                oc->player.d = true;
    	    }
    	    if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
                oc->player.d = false;
    	    }            
			if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
                camera.zoomIn = true;
            }
            if (key == GLFW_KEY_Q && action == GLFW_RELEASE) {
                camera.zoomIn = false;
            }
            if (key == GLFW_KEY_E && action == GLFW_PRESS) {
                camera.zoomOut = true;
            }
            if (key == GLFW_KEY_E && action == GLFW_RELEASE) {
                camera.zoomOut = false;
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

		pc->particleTexture = make_shared<Texture>();
		pc->particleTexture->setFilename(resourceDir + "/alpha.bmp");
		pc->particleTexture->init();
		pc->particleTexture->setUnit(4);
		pc->particleTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

//        roosterTexture = make_shared<Texture>();
//        roosterTexture->setFilename(resourceDir + "/prop_gas_station_baseColor.jpeg");
//        roosterTexture->init();
//        roosterTexture->setUnit(4);
//        roosterTexture->setWrapModes(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

	}

//    void gasSetUp(const std::string& resourceDirectory)
//    {
//        //initialize the textures we might use
//        roosterProg = make_shared<Program>();
//        roosterProg->setVerbose(true);
//        roosterProg->setShaderNames(
//                resourceDirectory + "/rooster_tex_vert.glsl",
//                resourceDirectory + "/rooster_tex_frag.glsl");
//        if (! roosterProg->init())
//        {
//            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
//            exit(1);
//        }
//        roosterProg->addUniform("P");
//        roosterProg->addUniform("V");
//        roosterProg->addUniform("M");
//        roosterProg->addUniform("Texture0");
//        roosterProg->addUniform("texNum");
//        roosterProg->addAttribute("vertPos");
//        roosterProg->addAttribute("vertNor");
//        roosterProg->addAttribute("vertTex");
//        roosterProg->addUniform("lightPos");
//    }

	void init()
	{
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		glClearColor(.12f, .34f, .56f, 1.0f);
		glEnable(GL_DEPTH_TEST);

        oc->setSoundEngine(soundEngine);
		sky.skySetUp();
//		gasSetUp(resourceDir);
		oc->objectSetUp();
		pc->setUp();
		terrain.initTerrain();
		water.initWater();
		shadow.init(width, height);
	}


	void initGeom()
	{
        oc->player.initPlayer(oc->gameplay);
		oc->initSceneCollectibles();
        oc->initSceneObjects();
		oc->initObjectHierarchy();
		butterfly.initbutterfly();

//        rooster = make_shared<Shape>();
//        rooster->loadMesh(resourceDir + "/gas_pump.obj");
//        rooster->resize();
//        rooster->init();

		CHECKED_GL_CALL(glGenVertexArrays(1, &pc->ParticleVertexArrayID));
		CHECKED_GL_CALL(glBindVertexArray(pc->ParticleVertexArrayID));

		CHECKED_GL_CALL(glGenBuffers(1, &pc->particlePointsBuffer));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pc->particlePointsBuffer));

		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(pc->points), NULL, GL_STREAM_DRAW));
		CHECKED_GL_CALL(glGenBuffers(1, &pc->particleColorBuffer));

		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pc->particleColorBuffer));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(pc->pointColors), NULL, GL_STREAM_DRAW));
	}



	void updateGeom(float dt)
	{
		glm::vec3 pos;
		glm::vec4 col;

		for (int i = 0; i < pc->numP; i++)
		{
			pos = pc->particles[i]->x;
			col = pc->particles[i]->color;
            pc->points[i * 3 + 0] = pos.x;
            pc->points[i * 3 + 1] = pos.y;
            pc->points[i * 3 + 2] = pos.z;
            pc->pointColors[i * 4 + 0] = col.r + col.a / 10.f;
            pc->pointColors[i * 4 + 1] = col.g + col.g / 10.f;
            pc->pointColors[i * 4 + 2] = col.b + col.b / 10.f;
            pc->pointColors[i * 4 + 3] = col.a;
		}


		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pc->particlePointsBuffer));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(pc->points), NULL, GL_STREAM_DRAW));
		CHECKED_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * pc->numP * 3, pc->points));

		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, pc->particleColorBuffer));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(pc->pointColors), NULL, GL_STREAM_DRAW));
		CHECKED_GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * pc->numP * 4, pc->pointColors));

		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	bool checkForEdge(vec3 hold)
	{
        for(int i = 0; i < oc->objects.size(); i++)
        {
            vec3 pos = oc->objects[i]->currentPos;
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

	float randFloat(float l, float h)
	{
		float r = rand() / (float) RAND_MAX;
		return (1.0f - r) * l + r * h;
	}


	void drawScene(MatrixStack* View, MatrixStack* Projection)
	{
		auto Model = make_shared<MatrixStack>();

        oc->objProg->bind();

        glUniformMatrix4fv(oc->objProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
        glUniformMatrix4fv(oc->objProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
        glUniform1f(oc->objProg->getUniform("numberLights"), lighting->numberLights);
        lighting->bind(oc->objProg->getUniform("lighting"));

        Model->pushMatrix();
        Model->loadIdentity();

        for(int i = 0; i < oc->objects.size(); i++)
        {
//            if(vfc->ViewFrustCull(oc->objects[i]->bs->midpt, -2.25))
//            {
                if (oc->objects[i]->type == GameObject::strawberry) {
					MatrixStack *modelptr = Model.get();
                    oc->objects[i]->drawObject(modelptr, oc->strawberryShapes, oc->objProg, camera.getPosition(),
                                               butterfly.currentPos, oc->gameplay);
                } else if (oc->objects[i]->type == GameObject::plant) {
                    CHECKED_GL_CALL(glEnable(GL_BLEND));
                    glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_DST_ALPHA);
//					Model->rotate(randFloat(0, 360), vec3(0,1,0));
					MatrixStack *modelptr = Model.get();
                    oc->objects[i]->drawObject(modelptr, oc->plantShapes, oc->objProg, camera.getPosition(),
                                               butterfly.currentPos, oc->gameplay);
                    CHECKED_GL_CALL(glDisable(GL_BLEND));
                }
//            }

        }

		oc->objProg->unbind();

		Model->popMatrix();
		lighting->unbind();

	}

//    void drawRooster(MatrixStack* View, MatrixStack* Projection)
//    {
//
//        auto Model = make_shared<MatrixStack>();
//        roosterProg->bind();
//        glUniformMatrix4fv(roosterProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
//        glUniformMatrix4fv(roosterProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
//        glUniform3f(roosterProg->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);
//
//        Model->pushMatrix();
//        Model->loadIdentity();
//
//        Model->translate(vec3(0, 2, 0));
//        Model->rotate(15, vec3(1,0,0));
//
//
//        Model->pushMatrix();
//        glUniformMatrix4fv(roosterProg->getUniform("M"), 1, GL_FALSE, value_ptr(Model->topMatrix()) );
//        roosterTexture->bind(roosterProg->getUniform("Texture0"));
//        glUniform1f(roosterProg->getUniform("texNum"), 1);
//        rooster->draw(roosterProg);
//        Model->popMatrix();
//        roosterTexture->unbind();
//
//        Model->popMatrix();
//        roosterProg->unbind();
//    }

    void render(float deltaTime)
    {
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// need mouse position in order to use camera
		double mousex = width / 4.0;
		double mousey = height / 4.0;    
		float aspect = width/(float)height;

		if(!releaseMouse)
			glfwGetCursorPos(windowManager->getHandle(), &mousex, &mousey);

        updateGeom(deltaTime);
  
        oc->player.updateView(deltaTime * 0.000001f, 
							mousex, 
							mousey, 
							width,
                            height, 
							camera.getPosition(), 
							oc->objects,
							oc->bvh);

		// TODO: make this better
		static float t = 0;
		if(t < 1)
		{
			glm::vec3 a = glm::vec3(0,0,0);
			glm::vec3 b = oc->player.currentPos;
			glm::vec3 control1 = glm::vec3(3.5,-2,7);
			glm::vec3 control2 = glm::vec3(-1,-3.5,9);
			butterfly.moveAlongPath(a, b, control1, control2, deltaTime, t);
			t += deltaTime*0.00000009;
			//std::cout << butterfly.center.x << "," << butterfly.center.y << "," << butterfly.center.z << std::endl;
		}
		else
		{		
			butterfly.updateModelMatrix(deltaTime, oc->player.currentPos);
		}
		

        lightPos.x = cos(glfwGetTime()/100) * 500.f;
        lightPos.z = sin(glfwGetTime()/100) * 500.f;

        auto ViewUser = make_shared<MatrixStack>();
        ViewUser->pushMatrix();
        ViewUser->loadIdentity();
        ViewUser->pushMatrix();

        if (!debug)
        {
            ViewUser->multMatrix(
                    camera.update(butterfly.center, deltaTime * 0.000001f,
                                  mousex, mousey, width, height));

            // reset mouse position to center of screen after finding difference.
            if (!releaseMouse)
                glfwSetCursorPos(windowManager->getHandle(), width / 4.0,
                                 height / 4.0);
        }
        else
        {
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


			cameraPos = holdCameraPos;
            ViewUser->lookAt(vec3(cameraPos.x, 1.0, cameraPos.z),
                             forward + vec3(cameraPos.x, 1.0, cameraPos.z), up);
        }

        MatrixStack *userViewPtr = ViewUser.get();


        auto Projection = make_shared<MatrixStack>();
        Projection->pushMatrix();
        Projection->perspective(radians(50.0f), aspect, 0.1f, 100.0f);
        MatrixStack *projectionPtr = Projection.get();

        CHECKED_GL_CALL(glDisable(GL_DEPTH_TEST));
        CHECKED_GL_CALL(glDisable(GL_BLEND));
        sky.drawSky(userViewPtr, projectionPtr, lightPos, glfwGetTime()/1000);

        // view frustum culling
        vfc->ExtractVFPlanes(Projection->topMatrix(), ViewUser->topMatrix());

        CHECKED_GL_CALL(glDisable(GL_BLEND));
        CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
        drawScene(userViewPtr, projectionPtr);

		auto Model = make_shared<MatrixStack>();
		Model->pushMatrix();
		Model->loadIdentity();
		Model->pushMatrix();


        if (!debug)
        {
            terrain.render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), shadow.getLS(), shadow.getDepthMap(), camera.getPosition(), lighting, butterfly.currentPos);
        }
        else
        {
            terrain.render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), shadow.getLS(), shadow.getDepthMap(), cameraPos, lighting, butterfly.currentPos);
        }

//        drawRooster(userViewPtr, projectionPtr);

        CHECKED_GL_CALL(glEnable(GL_BLEND));
        CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		water.render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), cameraPos);
		Model->popMatrix();

		shadow.render(oc->player, butterfly);
        CHECKED_GL_CALL(glDisable(GL_BLEND));
		oc->player.drawPlayer(userViewPtr, projectionPtr, camera.getPosition(), lighting, butterfly.currentPos);
		butterfly.drawbutterfly(userViewPtr, projectionPtr, camera.getPosition(), lighting);

        CHECKED_GL_CALL(glEnable(GL_BLEND));
        CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
        CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        CHECKED_GL_CALL(glPointSize(25.0f));
        pc->drawParticles(userViewPtr, aspect, keyToggles, oc->player.position, oc->gameplay, y);

		Projection->popMatrix();
		ViewUser->popMatrix();
		ViewUser->popMatrix();

    }

};

int main(int argc, char **argv)
{
	Application *application = new Application();

	irrklang::ISoundEngine* soundEngine = irrklang::createIrrKlangDevice();
    application->soundEngine = soundEngine;

	if (!soundEngine)
	{
		std::cerr << "Could not start irrKlang sound engine" << std::endl;
	}

    soundEngine->play2D("../resources/tame.ogg", true);
 
	WindowManager *windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	application->init();
	application->initTex();
	application->pc->initParticles();

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
