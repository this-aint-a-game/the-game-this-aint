
#include "Terrain.h"
#include "obtain_noise.h"

Terrain::Terrain()
{
    vertexArrayID = 0;
}

void Terrain::initTex()
{
    terrainTexture = std::make_shared<Texture>();
    terrainTexture->setFilename("../resources/Onefacegrey.jpg");
    terrainTexture->init();
    terrainTexture->setUnit(0);
    terrainTexture->setWrapModes(GL_REPEAT, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Terrain::render(glm::mat4 const & P, glm::mat4 const & V, glm::mat4 const & M, glm::vec3 cameraPos)
{
    prog->bind();

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));
    glUniform3f(prog->getUniform("cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
    glUniform3f(prog->getUniform("lightPos"), -2.0, 2.0, 2.0); // TODO

    // flat grey
    glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
    glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
    glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
    glUniform1f(prog->getUniform("shine"), 4.0);

    draw();

    //terrainTexture->unbind();
    prog->unbind();
}

void Terrain::computeIndicesForClipVolume(float clipx0, float clipx1, float clipz0, float clipz1)
{
    //std::cout << "81" << std::endl;
    int minCol = std::max(std::min((clipx0 / (float)STEP), (float)(WIDTH - 2)), 0.f);
    int maxCol = std::max(std::min((clipx1 +  STEP) / STEP, (float)(WIDTH - 2)), 0.f);
    int minRow = std::max(std::min(-clipz1 / STEP, (float)(DEPTH - 1)), 0.f);
    int maxRow = std::max(std::min(-(clipz0 - STEP) / STEP, (float)(DEPTH - 1)), 0.f);

    if (minCol == maxCol || minRow == maxRow)
    {
        std::cout << "terrain not visible" << std::endl;
        numIndices = 0;
    }

    unsigned index = 0;
    for (int c = minCol; c < maxCol + 1; c++)
    {
        for (int r = minRow; r < maxRow + 1; r++)
        {
            // if not first strip, link to previous strip
            if (c > minCol && r == minRow)
            {
                indices[index++] = c * DEPTH + r;
            }
            indices[index++] = c * DEPTH + r;
            indices[index++] = (c + 1) * DEPTH + r;

            // link to next strip
            if (r == maxRow && c < maxCol)
            {
                indices[index++] = (c + 1) * DEPTH + r;
            }
        }
        //std::cout << "indices: " << index << std::endl;
    }
    numIndices = index;
    //std::cout << "114" << std::endl;
}

void Terrain::bindVAO()
{
    //std::cout << "117" << std::endl;
    if (vertexArrayID == 0) {
        unsigned numVertices = vertices.size();
        unsigned vertexByteSize = sizeof(glm::vec3);
        unsigned vertexFloatSize = vertexByteSize / sizeof(float);

        unsigned bytes = numVertices * vertexByteSize;
        //uint8_t *vertexData = g_new(uint8_t, bytes);
        void *vertexData = malloc(bytes);
        //float *vertexDataCast = malloc(bytes);

        float *vertexDataCast = (float *) vertexData;
        for (int i = 0; i < numVertices; i++) {
            memcpy(&vertexDataCast[vertexFloatSize * i], &vertices[i],
                   vertexByteSize);
        }


        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, bytes, vertexData, GL_STATIC_DRAW);
        free(vertexData);

        // TODO how does num index buffers affect this
        glGenBuffers(1, &vertexIndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(int), indices,
                     GL_DYNAMIC_DRAW);


        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
    }
    else
    {
        glBindVertexArray(vertexArrayID);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    }


}

float Terrain::getHeight(float x, float z)
{
    return calcHeight(x, z);
}

float Terrain::calcHeight(float x, float z)
{
    float n = mountain_range_noise(glm::vec3(x, 0, z));
    return (0.5 + 0.5 * n) - 5;

}

void Terrain::generateGrid()
{
    int start = -WIDTH/2;//0;
    int finish = WIDTH/2;//0;

    for (int vx = start; vx < finish; vx++)
    {
        for (int vz = start; vz < finish; vz++)
        {
            // float vy = height[vx * depth + vz);?
            // #define TERRAIN(t, w, d) t->height[(w) * t->depth + (d)]
            //float vy = 0;
            float x = vx * STEP;
            float z = -vz * STEP;

            float y = calcHeight(x, z);
            //float y = 0.f;

            //glm::vec3 v0 = glm::vec3(vx * STEP, vy, -vz * STEP);
            glm::vec3 v0 = glm::vec3(x, y, z);

            // calculate normal?
            vertices.push_back(v0); // add normal?
        }
    }

    unsigned num_indices = (WIDTH - 1) * (DEPTH * 2) + (WIDTH - 2) + (DEPTH - 2);

    float clipx0 = 0.f;
    float clipx1 = (WIDTH - 1) * STEP;
    float clipz1 = 0.f;
    float clipz0 = -(DEPTH - 1) * STEP;

    //compute indices for clip volume
    indices = (unsigned *) malloc(num_indices * sizeof(unsigned));
    computeIndicesForClipVolume(clipx0, clipx1, clipz0, clipz1);

    assert(num_indices == numIndices);

    bindVAO();
}

void Terrain::clean()
{
    glBindVertexArray(0);
    glUseProgram(0);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &vertexIndexBuffer);
    glDeleteVertexArrays(1, &vertexArrayID);

}

void Terrain::draw()
{
    /*
    glBindVertexArray(vertexArrayID);
    glDrawElements(GL_TRIANGLES, VERTEX_COUNT * VERTEX_COUNT * 10, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
     */
    // TODO offset?
    bindVAO();
    glDrawElements(GL_TRIANGLE_STRIP, numIndices, GL_UNSIGNED_INT, 0);
    unbind();

}

void Terrain::unbind()
{
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Terrain::initTerrain()
{
    prog = std::make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames(
            "../resources/terrain_vert.glsl",
            "../resources/terrain_frag.glsl");
    if (! prog->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");

    //prog->addUniform("permTexture");
    //prog->addUniform("Texture");

    //initTex();

    prog->addUniform("MatAmb");
    prog->addUniform("MatDif");
    prog->addUniform("MatSpec");
    prog->addUniform("shine");
    prog->addUniform("lightPos");
    prog->addUniform("cameraPos");

    //prog->addUniform("mode");
    //prog->addUniform("freq");
    //prog->addUniform("octave");
    //prog->addUniform("power");

    prog->addAttribute("vertPos");
    //prog->addAttribute("vertNor");

    //Initialize textures
    //initPermTexture(&permTextureID);
}
