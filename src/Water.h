
#ifndef OBTAIN_WATER_H
#define OBTAIN_WATER_H

#define HEIGHT -4.8
#include "Program.h"
#include "MatrixStack.h"
#include "ColorCollectGameplay.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Water {

    ColorCollectGameplay * ccg;

    std::shared_ptr<Program> prog;
    std::vector<glm::vec3> vertices;
    unsigned *indices;

    GLuint vertexArrayID;
    GLuint vertexBuffer;
    GLuint vertexIndexBuffer;
    unsigned numIndices;

    void draw();
    void unbind();
    void bindVAO();
    void generateGrid(int width);
    void computeIndicesForClipVolume(int width, float clipx0, float clipx1, float clipz0, float clipz1);

public:
    Water(ColorCollectGameplay * ccg);
    void render(glm::mat4 const & P, glm::mat4 const & V, glm::mat4 const & M, glm::vec3 cameraPos);
    void initWater();
};


#endif //OBTAIN_WATER_H
