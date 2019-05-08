#include "Player.h"
#include "Terrain.h"
#include "Strawberry.h"

glm::mat4 Player::updateModelMatrix(double frametime, int mousex, int mousey,
                            int width, int height)
{
    float speed = 0;
    float lateralSpeed = 0;
    float yawAngle = 0;
    float pitchAngle = 0;
    if (w == 1)
        speed = MOVESPEED * frametime;
    else if (s == 1)
        speed = -MOVESPEED * frametime;
    if(a == 1)
        lateralSpeed = -MOVESPEED * frametime;
    else if(d == 1)
        lateralSpeed = MOVESPEED * frametime;

    glm::mat4 Ry = glm::rotate(glm::mat4(1), yaw.y, glm::vec3(0, 1, 0));
    glm::mat4 Rx = glm::rotate(glm::mat4(1), pitch.x, glm::vec3(1, 0, 0));
    glm::vec4 dir = glm::vec4(0, 0, speed, 1);

    dir = dir * Rx * Ry;
    
    glm::vec3 lateralDir = glm::vec3(glm::vec4(0,0, lateralSpeed, 1) * 
                glm::rotate(glm::mat4(1), 3.14159f / 2.0f, glm::vec3(0,1,0))
                           *Ry);
    targetPos += lateralDir;
    targetPos += glm::vec3(dir.x, 0, dir.z);
    position += -0.1f * position + 0.1f * targetPos;
    position.y = Terrain::getHeight(position.x, position.z) + 0.4;

    currentPos = position;
    
    glm::mat4 T = glm::translate(glm::mat4(1), position);
    return Rx * Ry * T;
}

void Player::initPlayer(ColorCollectGameplay * ccg)
{
    playerProg = std::make_shared<Program>();
    playerProg->setVerbose(true);
    playerProg->setShaderNames(
            "../Resources/phong_vert.glsl",
            "../Resources/phong_frag.glsl");
    if (! playerProg->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    playerProg->addUniform("P");
    playerProg->addUniform("V");
    playerProg->addUniform("M");
    playerProg->addAttribute("vertPos");
    playerProg->addAttribute("vertNor");
    playerProg->addUniform("MatAmb");
    playerProg->addUniform("MatDif");
    playerProg->addUniform("MatSpec");
    playerProg->addUniform("shine");
    playerProg->addUniform("view");
    playerProg->addUniform("numberLights");
    playerProg->addUniform("lighting");


    // Initialize the obj mesh VBOs etc
    playerShape = std::make_shared<Shape>();
    playerShape->loadMesh("../Resources/sphere.obj");
    playerShape->resize();
    playerShape->init();

    scale = glm::vec3(0.2,0.2,0.2);

    this->bb = new BoundingBox(playerShape->min, playerShape->max);
}

void Player::drawPlayer(MatrixStack* View, MatrixStack* Projection, glm::vec3 view, Lighting* lighting)
{
    playerProg->bind();

    glUniformMatrix4fv(playerProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
    glUniformMatrix4fv(playerProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
    glUniformMatrix4fv(playerProg->getUniform("M"), 1, GL_FALSE, (GLfloat*)&model);
    glUniform3f(playerProg->getUniform("view"), view.x, view.y, view.z);
    glUniform3f(playerProg->getUniform("MatAmb"), 0.13, 0.13, 0.14);
    glUniform3f(playerProg->getUniform("MatDif"), 0.3, 0.3, 0.4);
    glUniform3f(playerProg->getUniform("MatSpec"), 0.3, 0.3, 0.4);
    glUniform1f(playerProg->getUniform("shine"), 47.0);
    glUniform1f(playerProg->getUniform("numberLights"), lighting->numberLights);
    lighting->bind(playerProg->getUniform("lighting"));

    playerShape->draw(playerProg);

    playerProg->unbind();
}

void Player::updateView(double frametime, int mousex, int mousey, int width, int height)
{
    model = updateModelMatrix(frametime, mousex, mousey, width, height);
    model *= glm::scale(glm::mat4(1), scale);
}

bool Player::checkForCollisions(std::vector<GameObject*> & objs)
{
    //std::cout << "97" << std::endl;
    for(int i = 0; i < objs.size(); i++)
    {
        if (objs[i]->isCollided(getBB()))
        {
            //std::cout << "102" << std::endl;
            if (dynamic_cast<Strawberry*>(objs[i]) != nullptr)
            {
                //std::cout << "105" << std::endl;
                auto s = dynamic_cast<Strawberry*>(objs[i]);
                std::cout << "collecting" << std::endl;
                s->collect();

                // TODO free?
                objs.erase(objs.begin()+i);
            }
            return true;
        }
    }
    //std::cout << "115" << std::endl;
    return false;
}
