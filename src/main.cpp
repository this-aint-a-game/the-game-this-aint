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
#include "Terrain.h"
#include "Water.h"
#include "Shadow.h"
#include "Camera.h"
#include "Lighting.h"
#include "Sky.h"
#include "ObjectCollection.h"
#include "ParticleCollection.h"

#define MOVEMENT_SPEED 0.2f
#define RENDER_SPEED 0.5f

using namespace std;
using namespace glm;

bool debug = false;

class Application : public EventCallbacks
{

public:

    ParticleCollection *pc = new ParticleCollection();
    ObjectCollection *oc = new ObjectCollection();

	Camera camera = Camera();
    Lighting* lighting = new Lighting();
	Shadow shadow = Shadow(lighting);
	Terrain terrain = Terrain(oc->gameplay);
	Water water = Water(oc->gameplay);
	Sky sky = Sky();

	WindowManager * windowManager = nullptr;
	int width, height;

	bool releaseMouse = false;
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

	}

	void init()
	{
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		GLSL::checkVersion();

		glClearColor(.12f, .34f, .56f, 1.0f);
		glEnable(GL_DEPTH_TEST);

		sky.skySetUp();
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
		oc->initObjectHierarchy();
//        initSceneObjects();

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
            MatrixStack *modelptr = Model.get();

            if(oc->objects[i]->type == GameObject::strawberry)
            {
                oc->objects[i]->drawObject(modelptr, oc->strawberryShapes, oc->objProg, camera.getPosition());
            }
            else if(oc->objects[i]->type == GameObject::crystal1)
            {
                CHECKED_GL_CALL(glEnable(GL_BLEND));
                glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);
                oc->objects[i]->drawObject(modelptr, oc->crystal1Shapes, oc->objProg, camera.getPosition());
                CHECKED_GL_CALL(glDisable(GL_BLEND));
            }

        }

		oc->objProg->unbind();

		Model->popMatrix();
		lighting->unbind();

	}

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

        oc->player.updateView(deltaTime * 0.000001f, mousex, mousey, width,
						  height, camera.getPosition(), oc->objects);
        oc->player.checkForCollisions(oc->objects, oc->bvh);

        lightPos.x = cos(glfwGetTime()/100) * 500.f;
        lightPos.z = sin(glfwGetTime()/100) * 500.f;

        auto ViewUser = make_shared<MatrixStack>();
        ViewUser->pushMatrix();
        ViewUser->loadIdentity();
        ViewUser->pushMatrix();



        if (!debug)
        {
            ViewUser->multMatrix(
                    camera.update(oc->player.position, deltaTime * 0.000001f,
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
            terrain.render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), shadow.getLS(), shadow.getDepthMap(), camera.getPosition(), lighting);
        }
        else
        {
            terrain.render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), shadow.getLS(), shadow.getDepthMap(), cameraPos, lighting);
        }

        CHECKED_GL_CALL(glEnable(GL_BLEND));
        CHECKED_GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		water.render(Projection->topMatrix(), ViewUser->topMatrix(), Model->topMatrix(), cameraPos);
		Model->popMatrix();

		shadow.render(oc->player);
        CHECKED_GL_CALL(glDisable(GL_BLEND));
		oc->player.drawPlayer(userViewPtr, projectionPtr, camera.getPosition(), lighting);

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
 
	WindowManager *windowManager = new WindowManager();
	windowManager->init(1024, 1024);
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
