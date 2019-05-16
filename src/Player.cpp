#include "Player.h"
#include "Terrain.h"
#include "Strawberry.h"

glm::mat4 Player::updateModelMatrix(double frametime, 
                                    int mousex, 
                                    int mousey,
                                    int width, 
                                    int height,
                                    glm::vec3 camPos)
{
    oldPos = position;
    float speed = 0;
    float lateralSpeed = 0;
    glm::vec3 camDir = glm::normalize(glm::vec3(position.x - camPos.x, 0, position.z - camPos.z));
    
    if (w == true) 
    {
        speed = MOVESPEED * frametime;
        targetDir = camDir;
    }
    else if(s == true) 
    {
        speed = MOVESPEED * frametime;
        targetDir = glm::vec4(camDir, 1) * glm::rotate(glm::mat4(1), PI, glm::vec3(0,1,0));
    }
    else if(a == true)
    {
        speed = MOVESPEED * frametime;       
        targetDir = glm::vec4(camDir, 1) * glm::rotate(glm::mat4(1), -PI / 2, glm::vec3(0,1,0));
    }
    else if(d == true)
    {
        speed = MOVESPEED * frametime;
        targetDir = glm::vec4(camDir, 1) * glm::rotate(glm::mat4(1), PI / 2, glm::vec3(0,1,0));
    }
 
    dir += (float)(frametime * PLAYER_ROTATION_SPRING) * (targetDir - dir);
    targetYaw = atan2(targetDir.x, targetDir.z);
    yaw += frametime * PLAYER_ROTATION_SPRING * (targetYaw - yaw);
    glm::mat4 Ry = glm::rotate(glm::mat4(1), yaw, glm::vec3(0,1,0));

    position += speed * dir;
    position.y = Terrain::getHeight(position.x, position.z) + 0.3;

    currentPos = position;
    
    return glm::translate(glm::mat4(1), position) * glm::rotate(glm::mat4(1), yaw, glm::vec3(0,1,0));
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
    playerShape->loadMesh("../Resources/character.obj");
    playerShape->resize();
    playerShape->init();

    scale = glm::vec3(0.2,0.2,0.2);

    this->bb = new BoundingBox(playerShape->min, playerShape->max, scale);
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

void Player::updateView(double frametime, 
                        int mousex, 
                        int mousey, 
                        int width, 
                        int height, 
                        glm::vec3 camdir)
{
    model = updateModelMatrix(frametime, mousex, mousey, width, height, camdir);
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
            /*
            if(glm::distance(oldPos, objs[i]->currentPos) < glm::distance(position, objs[i]->currentPos))
            {
                std::cout << "1" << endl;
                position = targetPos = oldPos;
                model = glm::translate(glm::mat4(1), oldPos) * glm::scale(glm::mat4(1), scale);
            }
            */
            return true;
        }
    }
    return false;
}
