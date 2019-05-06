#include "Player.h"
#include "Terrain.h"
#define MOVESPEED 0.5

glm::mat4 Player::update(double frametime, int mousex, int mousey,
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
    position.y = Terrain::getHeight(position.x, position.z) + 0.2;
    
    glm::mat4 T = glm::translate(glm::mat4(1), position);
    return Rx * Ry * T;
}

void Player::initPlayer()
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
}

void Player::drawPlayer(MatrixStack* View, MatrixStack* Projection, glm::mat4* M)
{
    playerProg->bind();

    glUniformMatrix4fv(playerProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
    glUniformMatrix4fv(playerProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
    glUniformMatrix4fv(playerProg->getUniform("M"), 1, GL_FALSE, (GLfloat*)M);
    //glUniform3f(playerProg->getUniform("lightPos"), lightPos.x, lightPos.y, lightPos.z);

    playerShape->draw(playerProg);

    playerProg->unbind();
}
