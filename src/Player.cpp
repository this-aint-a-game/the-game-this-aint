#include "Player.h"
#include "Terrain.h"
#include "Strawberry.h"

glm::mat4 Player::updateModelMatrix(double frametime, 
                                    int mousex, 
                                    int mousey,
                                    int width, 
                                    int height,
                                    glm::vec3 camPos,
                                    std::vector<GameObject*> & objs,
                                    BoundingVolumeHierarchy * bvh)
{
    animationTimer += 6*frametime;
    if(animationTimer >= 9)     
    {
        if(inSomersault)
            inSomersault = false;
        animationTimer = 0;
    }
    // set speed = 0 as default, will update value based on keypresses
    float speed = 0;
    // because the camera is always looking at the player when the player has control of the 
    // camera, we can assume that the camera's direction is the vector between the camera 
    // and the player
    glm::vec3 camDir = glm::normalize(glm::vec3(position.x - camPos.x, 0, position.z - camPos.z));
    
    // set speed and target direction based on keypresses
    if (w == true) 
    {
        if(sprint)
            speed = SPRINTSPEED * frametime;
        else
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
 
    // lerp the direction towards the target direction
    dir += (float)(frametime * PLAYER_ROTATION_SPRING) * (targetDir - dir);
    // extract the target angle from the target direction
    targetYaw = atan2(targetDir.x, targetDir.z);
    // lerp the yaw angle towards the target angle
    yaw += frametime * PLAYER_ROTATION_SPRING * (targetYaw - yaw);
    // construct a rotation matrix based on the currect yaw
    glm::mat4 Ry = glm::rotate(glm::mat4(1), yaw, glm::vec3(0,1,0));

    // move the player in the direction they are facing
    currentPos = position + (speed * dir);
    // set their y position based on the height of the terrain at that point, with temporary bouncing until we have animations
    currentPos.y = Terrain::getHeight(currentPos.x, currentPos.z) + 0.3 + ((1+sin(timer += frametime*4))/8);

    //std::cout << "position prior to check is: " << position.x << "," << position.y << "," << position.z << std::endl;
    if(checkForCollisions(objs, bvh))
        currentPos = position;
    else
        position = currentPos;

    // construct a model matrix using our current position and current yaw angle
    return glm::translate(glm::mat4(1), position) * Ry;
}

void Player::initPlayer(ColorCollectGameplay * ccg)
{
    playerProg = std::make_shared<Program>();
    playerProg->setVerbose(true);
    playerProg->setShaderNames(
            "../resources/player_vert.glsl",
            "../resources/player_frag.glsl");
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
    playerProg->addUniform("butterflyPos");
/*
    playerProg->addUniform("MatAmb");
    playerProg->addUniform("MatDif");
    playerProg->addUniform("MatSpec");
    playerProg->addUniform("shine");
    playerProg->addUniform("view");
    playerProg->addUniform("numberLights");
    playerProg->addUniform("lighting");
    */


    // Initialize the obj mesh VBOs etc
    playerShape = std::make_shared<Shape>();
    playerShape->loadMesh("../resources/character.obj");
    playerShape->resize();
    playerShape->init();

    scale = glm::vec3(0.2,0.2,0.2);

    this->bs = new BoundingSphere(playerShape->min, playerShape->max, scale, 0.f);

    // animations
    for(int i = 0; i < 9; i++)
    {
        idleframes.push_back(std::make_shared<Shape>());
        idleframes[i]->loadMesh("../resources/frame" + std::to_string(i) + ".obj");
        idleframes[i]->resize();
        idleframes[i]->init();
    }
        for(int i = 0; i < 9; i++)
    {
        somersaultframes.push_back(std::make_shared<Shape>());
        somersaultframes[i]->loadMesh("../resources/somersault" + std::to_string(i) + ".obj");
        somersaultframes[i]->resize();
        somersaultframes[i]->init();
    }
}

void Player::drawPlayer(MatrixStack* View, MatrixStack* Projection, glm::vec3 view, Lighting* lighting, glm::vec3 butterflyPos)
{
    playerProg->bind();

    glUniformMatrix4fv(playerProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
    glUniformMatrix4fv(playerProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
    glUniformMatrix4fv(playerProg->getUniform("M"), 1, GL_FALSE, (GLfloat*)&model);    
    glUniform3f(playerProg->getUniform("butterflyPos"), butterflyPos.x, butterflyPos.y, butterflyPos.z);
 
 /*
    glUniform3f(playerProg->getUniform("view"), view.x, view.y, view.z);
    glUniform3f(playerProg->getUniform("MatAmb"), 0.13, 0.13, 0.14);
    glUniform3f(playerProg->getUniform("MatDif"), 0.3, 0.3, 0.4);
    glUniform3f(playerProg->getUniform("MatSpec"), 0.3, 0.3, 0.4);
    glUniform1f(playerProg->getUniform("shine"), 47.0);
    glUniform1f(playerProg->getUniform("numberLights"), lighting->numberLights);
    lighting->bind(playerProg->getUniform("lighting"));
*/
    //playerShape->draw(playerProg);
    if(!inSomersault)
        idleframes[(int)animationTimer]->draw(playerProg);
    else
        somersaultframes[(int)animationTimer]->draw(playerProg);

    playerProg->unbind();
}

void Player::updateView(double frametime, 
                        int mousex, 
                        int mousey, 
                        int width, 
                        int height, 
                        glm::vec3 camdir, 
                        std::vector<GameObject*> & objs,
                        BoundingVolumeHierarchy * bvh)
{
    model = updateModelMatrix(frametime, mousex, mousey, width, height, camdir, objs, bvh);
    model *= glm::scale(glm::mat4(1), scale);
}
void Player::drawShape(std::shared_ptr<Program> prog)
{
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, (GLfloat*)&model);
    //playerShape->draw(prog);
    if(!inSomersault)
        idleframes[(int)animationTimer]->draw(prog);
    else
        somersaultframes[(int)animationTimer]->draw(prog);
}

bool Player::checkForCollisions(std::vector<GameObject*> &objs, BoundingVolumeHierarchy* bvh)
{
    bool collided = false;
    bvh->checkForCollision(objs, bvh->root, this->getBS(), collided);

    if(collided)
    {
        return true;
    }

    return false;
}
void Player::doSomersault()
{
    inSomersault = true;
    animationTimer = 0;
}
