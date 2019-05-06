
#ifndef OBTAIN_TERRAIN_H
#define OBTAIN_TERRAIN_H

#define INITIAL_WIDTH 100
#define WIDTH 700
#define STEP 0.5f

#include <memory>
#include "Program.h"
#include "MatrixStack.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>
#include <thread>

#include "GLSL.h"
#include <glad/glad.h>
#include <vector>

#include "GLTextureWriter.h"

class Terrain {

    std::vector<glm::vec3> vertices;
    unsigned *indices;
    unsigned numIndices;
    std::shared_ptr<Texture> terrainTexture;
    //unsigned indexBuffer[3]; // TODO 3?

    std::shared_ptr<Program> prog;
    GLuint vertexArrayID;
    GLuint vertexBuffer;
    GLuint vertexIndexBuffer;
    //GLuint numIndices;

    void initTex();
    void unbind();
    void clean();
    void draw();
    void computeIndicesForClipVolume(int width, float clipx0, float clipx1, float clipz0, float clipz1);
    static float calcHeight(float x, float z);
    std::thread threadedGenerateGrid();
    void generateGrid(int width = WIDTH);
    void bindVAO();

public:
    Terrain();
    void render(glm::mat4 const & P, glm::mat4 const & V, glm::mat4 const & M, glm::vec3 cameraPos);
    void initTerrain();
    static float getHeight(float x, float z);

};


#endif //OBTAIN_TERRAIN_H
