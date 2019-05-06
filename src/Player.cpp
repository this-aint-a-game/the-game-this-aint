#include "Player.h"
#include "Terrain.h"
#include "Strawberry.h"
#define MOVESPEED 0.5

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
            "../Resources/player_vert.glsl",
            "../Resources/player_frag.glsl");
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
    playerProg->addAttribute("vertTex");

    // Initialize the obj mesh VBOs etc
    playerShape = std::make_shared<Shape>();
    playerShape->loadMesh("../Resources/character.obj");
    playerShape->resize();
    playerShape->init();

    initObject(playerShape->min, playerShape->max, 0, GameObject::player, ccg);
}

void Player::initObject(glm::vec3 min, glm::vec3 max, int num, objType type, ColorCollectGameplay * ccg)
{
    this->bb = new BoundingBox(min, max);
}

void Player::drawPlayer(MatrixStack* View, MatrixStack* Projection)
{
    playerProg->bind();

    glUniformMatrix4fv(playerProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
    glUniformMatrix4fv(playerProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
    glUniformMatrix4fv(playerProg->getUniform("M"), 1, GL_FALSE, (GLfloat*)&model);
    //glUniform3f(playerProg->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);

    playerShape->draw(playerProg);

    playerProg->unbind();
}

void Player::updateView(double frametime, int mousex, int mousey, int width, int height)
{
    model = updateModelMatrix(frametime, mousex, mousey, width, height);
    model *= glm::scale(glm::mat4(1), glm::vec3(0.2,0.2,0.2));
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
