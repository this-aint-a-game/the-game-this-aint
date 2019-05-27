#include "Butterfly.h"
#include "Player.h"

#define BUTTERFLY_ANGLE_SCALE 2.0
#define BUTTERFLY_HEIGHT 2.0
#define BUTTERFLY_SPEED 0.0000008f

void Butterfly::updateModelMatrix(double frametime, glm::vec3 playerPos)
{
    angle += frametime *  BUTTERFLY_SPEED * BUTTERFLY_ANGLE_SCALE;
    offsets = glm::vec3(cos(angle), sin(5*angle)*0.05 + BUTTERFLY_HEIGHT, sin(angle));
    currentPos = playerPos + offsets;
    model = glm::translate(glm::mat4(1), currentPos) 
            * glm::rotate(glm::mat4(1), -1*angle, glm::vec3(0,1,0)) 
            * glm::scale(glm::mat4(1), glm::vec3(0.03f,0.03f,0.03f));
}

void Butterfly::initbutterfly()
{
    butterflyProg = std::make_shared<Program>();
    butterflyProg->setVerbose(true);
    butterflyProg->setShaderNames(
            "../Resources/butterfly_vert.glsl",
            "../Resources/butterfly_frag.glsl");
    if (! butterflyProg->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    butterflyProg->addUniform("P");
    butterflyProg->addUniform("V");
    butterflyProg->addUniform("M");
    butterflyProg->addAttribute("vertPos");
    butterflyProg->addAttribute("vertNor");
    butterflyProg->addAttribute("vertTex");

    // Initialize the obj mesh VBOs etc
    butterflyShape = std::make_shared<Shape>();
    butterflyShape->loadMesh("../Resources/butterfly.obj");
    butterflyShape->resize();
    butterflyShape->init();    
    // Initialize the obj mesh VBOs etc
    butterflyShapeWingsUp = std::make_shared<Shape>();
    butterflyShapeWingsUp->loadMesh("../Resources/butterflyWingsUp.obj");
    butterflyShapeWingsUp->resize();
    butterflyShapeWingsUp->init();
}

void Butterfly::drawbutterfly(MatrixStack* View, MatrixStack* Projection, glm::vec3 view, Lighting* lighting)
{
    butterflyProg->bind();

    glUniformMatrix4fv(butterflyProg->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
    glUniformMatrix4fv(butterflyProg->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
    glUniformMatrix4fv(butterflyProg->getUniform("M"), 1, GL_FALSE, (GLfloat*)&model);
   /* glUniform3f(butterflyProg->getUniform("view"), view.x, view.y, view.z);
    glUniform3f(butterflyProg->getUniform("MatAmb"), 0.13, 0.13, 0.14);
    glUniform3f(butterflyProg->getUniform("MatDif"), 0.3, 0.3, 0.4);
    glUniform3f(butterflyProg->getUniform("MatSpec"), 0.3, 0.3, 0.4);
    glUniform1f(butterflyProg->getUniform("shine"), 47.0);
    glUniform1f(butterflyProg->getUniform("numberLights"), lighting->numberLights);
    lighting->bind(butterflyProg->getUniform("lighting"));
*/
    if(sin(5*angle) < 0.75)
        butterflyShape->draw(butterflyProg);
    else
        butterflyShapeWingsUp->draw(butterflyProg);
    
    butterflyProg->unbind();
}

void Butterfly::drawShape(std::shared_ptr<Program> prog)
{
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, (GLfloat*)&model);
    butterflyShape->draw(prog);
}

