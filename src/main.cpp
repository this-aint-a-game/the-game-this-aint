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
#include "Moon.h"
#include "Bloom.h"
#include <irrKlang.h>

#define MOVEMENT_SPEED 0.2f
#define RENDER_SPEED 0.5f
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 540

using namespace std;
using namespace glm;

bool debug = false;
int first = 0;

class Application : public EventCallbacks
{

public:

    irrklang::ISoundEngine* soundEngine;
    ParticleCollection *pc = new ParticleCollection();
    ObjectCollection *oc = new ObjectCollection();
    Moon *moon = new Moon();
	Butterfly butterfly = Butterfly();
	Camera camera = Camera();
    Lighting* lighting = new Lighting();
	Shadow shadow = Shadow();
	Terrain terrain = Terrain(oc->gameplay);
	Water water = Water(oc->gameplay);
	Sky sky = Sky();
	Bloom bloom = Bloom();
	ViewFrustumCulling* vfc = new ViewFrustumCulling();

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
			if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
				oc->player.sprint = true;
			} 			
			if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
				oc->player.sprint = false;
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
	}

	void init()
	{
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		//glClearColor(.12f, .34f, .56f, 1.0f);
		glEnable(GL_DEPTH_TEST);

        oc->setSoundEngine(soundEngine);
        oc->setCamera(&camera);
		sky.skySetUp();
//		gasSetUp(resourceDir);
		oc->objectSetUp();
		pc->setUp();
		terrain.initTerrain();
		water.initWater();
		int w, h;
        glfwGetFramebufferSize(windowManager->getHandle(), &w, &h);
        bloom.init(w, h);
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
        //bloom.render(butterfly, oc, View, Projection, camera.getPosition());
	    oc->drawScene(oc->objProg, View, Projection, camera.getPosition(), butterfly.currentPos);
        moon->drawObject(View, Projection, camera.getPosition(), butterfly.currentPos, oc->gameplay);
	}

    void render(float deltaTime)
    {
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        glViewport(0, 0, width, height);

        glBindFramebuffer(GL_FRAMEBUFFER, bloom.getScreenBuf());
       // glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        //glEnable(GL_DEPTH_TEST);
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
		/*
		if(t < 1)
		{
			glm::vec3 a = glm::vec3(0,0,0);
			glm::vec3 b = oc->player.currentPos;
			glm::vec3 control1 = glm::vec3(3.5,-2,7);
			glm::vec3 control2 = glm::vec3(-1,-3.5,9);
			butterfly.moveAlongPath(a, b, control1, control2, deltaTime, t);
			t += deltaTime*0.0000001;
			//std::cout << butterfly.center.x << "," << butterfly.center.y << "," << butterfly.center.z << std::endl;
		}

		else
		{
		 */
			butterfly.updateModelMatrix(deltaTime, oc->player.currentPos);
		//}


//        std::cout << oc->player.currentPos.x << "," << oc->player.currentPos.y << "," << oc->player.currentPos.z << std::endl;

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

        CHECKED_GL_CALL(glEnable(GL_BLEND));
        CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		water.render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), cameraPos);
		Model->popMatrix();

		//shadow.render(butterfly, oc, userViewPtr, projectionPtr, camera.getPosition());
        CHECKED_GL_CALL(glDisable(GL_BLEND));
		oc->player.drawPlayer(userViewPtr, projectionPtr, camera.getPosition(), lighting, butterfly.currentPos);

        butterfly.drawbutterfly(butterfly.butterflyProg, userViewPtr, projectionPtr, camera.getPosition(), oc->gameplay);


        CHECKED_GL_CALL(glEnable(GL_BLEND));
        CHECKED_GL_CALL(glEnable(GL_DEPTH_TEST));
        CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        CHECKED_GL_CALL(glPointSize(25.0f));
        pc->drawParticles(userViewPtr, aspect, keyToggles, oc->player.position, oc->gameplay, y);


		Projection->popMatrix();
		ViewUser->popMatrix();
		ViewUser->popMatrix();

		/*
		if (first % 20 == 0)
        {
            std::string filename = "screen" + std::to_string(first) + ".png";

			assert(GLTextureWriter::WriteImage(bloom.getScreenBuf(), filename));
        }
        */


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        bloom.combine();
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

    //soundEngine->play2D("../resources/tame.ogg", true);
 
	WindowManager *windowManager = new WindowManager();
	windowManager->init(WINDOW_WIDTH, WINDOW_HEIGHT);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	application->init();
	application->initTex();
	application->pc->initParticles();
	application->moon->initMoon();

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
