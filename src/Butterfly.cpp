#include "Butterfly.h"
#include "Terrain.h"

#define BUTTERFLY_ANGLE_SCALE 2.0
#define BUTTERFLY_HEIGHT 1.75
#define BUTTERFLY_DISTANCE 0.5
#define BUTTERFLY_SPEED 0.000001f
#define BUTTERFLY_SIZE 0.09

void Butterfly::updateModelMatrix(double frametime, glm::vec3 origin)
{
    angle += frametime *  BUTTERFLY_SPEED * BUTTERFLY_ANGLE_SCALE;
    offsets = glm::vec3(BUTTERFLY_DISTANCE * cos(angle), /*sin(5*angle)*0.05 +*/ BUTTERFLY_HEIGHT, BUTTERFLY_DISTANCE * sin(angle));
    currentPos = origin + offsets;
    currentPos.y = std::max((float)(Terrain::getHeight(currentPos.x, currentPos.z) + BUTTERFLY_HEIGHT), currentPos.y);
    model = glm::translate(glm::mat4(1), currentPos) 
            * glm::rotate(glm::mat4(1), -1*angle, glm::vec3(0,1,0)) 
            * glm::scale(glm::mat4(1), glm::vec3(BUTTERFLY_SIZE));
    center = origin;
    center.y = (origin.y + currentPos.y) / 2.0;
}

void Butterfly::moveAlongPath(glm::vec3 a, glm::vec3 b, glm::vec3 control1, glm::vec3 control2, double frametime, double t)
{
    glm::vec3 origin = Bezier::cubeBez(Bezier::quadErp, a, b, control1, control2, t);
    this->updateModelMatrix(frametime, origin);
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
    butterflyProg->addUniform("bloom");
    butterflyProg->addUniform("bloomBuf");
    butterflyProg->addUniform("MatDif");
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

void Butterfly::drawbutterfly(std::shared_ptr<Program> prog, MatrixStack* View, MatrixStack* Projection, glm::vec3 view, ColorCollectGameplay* ccg)
{
    prog->bind();

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(Projection->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(View->topMatrix()));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, (GLfloat*)&model);

    if (ccg->checkColor(2)) {

        glm::vec3 d = ccg->yellowColor.diffuse;
        glUniform3f(prog->getUniform("MatDif"), d.x, d.y, d.z);
    } else{
        glUniform3f(prog->getUniform("MatDif"), 1, 1, 1);
    }

   /* glUniform3f(butterflyProg->getUniform("view"), view.x, view.y, view.z);
    glUniform3f(butterflyProg->getUniform("MatAmb"), 0.13, 0.13, 0.14);
    glUniform3f(butterflyProg->getUniform("MatDif"), 0.3, 0.3, 0.4);
    glUniform3f(butterflyProg->getUniform("MatSpec"), 0.3, 0.3, 0.4);
    glUniform1f(butterflyProg->getUniform("shine"), 47.0);
    glUniform1f(butterflyProg->getUniform("numberLights"), lighting->numberLights);
    lighting->bind(butterflyProg->getUniform("lighting"));
*/
    if(sin(5*angle) < 0.75)
        butterflyShape->draw(prog);
    else
        butterflyShapeWingsUp->draw(prog);
    
    prog->unbind();
}

void Butterfly::drawShape(std::shared_ptr<Program> prog)
{
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, (GLfloat*)&model);
    butterflyShape->draw(prog);
}

