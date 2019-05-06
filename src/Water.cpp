
#include "Water.h"
#include "Terrain.h"

Water::Water()
{
    vertexArrayID = 0;
    generateGrid(WIDTH);
}

void Water::generateGrid(int width)
{
    int start = -width/2;
    int finish = width/2;

    for (int vx = start; vx < finish; vx++)
    {
        for (int vz = start; vz < finish; vz++)
        {
            float x = vx * STEP;
            float y = HEIGHT;
            float z = -vz * STEP;

            glm::vec3 v0 = glm::vec3(x, y, z);

            // TODO calculate normal

            vertices.push_back(v0);
        }
    }

    unsigned num_indices = (width - 1) * (width * 2) + (width - 2) + (width - 2);

    float clipx0 = 0.f;
    float clipx1 = (width - 1) * STEP;
    float clipz1 = 0.f;
    float clipz0 = -(width - 1) * STEP;

    //compute indices for clip volume
    indices = (unsigned *) malloc(num_indices * sizeof(unsigned));
    computeIndicesForClipVolume(width, clipx0, clipx1, clipz0, clipz1);

    assert(num_indices == numIndices);
}


void Water::computeIndicesForClipVolume(int width, float clipx0, float clipx1, float clipz0, float clipz1)
{
    int minCol = std::max(std::min((clipx0 / (float)STEP), (float)(width - 2)), 0.f);
    int maxCol = std::max(std::min((clipx1 +  STEP) / STEP, (float)(width - 2)), 0.f);
    int minRow = std::max(std::min(-clipz1 / STEP, (float)(width - 1)), 0.f);
    int maxRow = std::max(std::min(-(clipz0 - STEP) / STEP, (float)(width - 1)), 0.f);

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
                indices[index++] = c * width + r;
            }
            indices[index++] = c * width + r;
            indices[index++] = (c + 1) * width + r;

            // link to next strip
            if (r == maxRow && c < maxCol)
            {
                indices[index++] = (c + 1) * width + r;
            }
        }
    }
    numIndices = index;
}

void Water::draw()
{
    bindVAO();
    glDrawElements(GL_TRIANGLE_STRIP, numIndices, GL_UNSIGNED_INT, 0);
    //glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
    unbind();
}

void Water::unbind()
{
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Water::bindVAO()
{
    if (vertexArrayID == 0) {
        unsigned numVertices = vertices.size();

        unsigned vertexByteSize = sizeof(glm::vec3);
        unsigned vertexFloatSize = vertexByteSize / sizeof(float);

        unsigned bytes = numVertices * vertexByteSize;
        void *vertexData = malloc(bytes);

        float *vertexDataCast = (float *) vertexData;
        for (int i = 0; i < numVertices; i++) {
            memcpy(&vertexDataCast[vertexFloatSize * i], &vertices[i],
                   vertexByteSize);
        }

        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, bytes, vertexData, GL_STATIC_DRAW);
        free(vertexData);

        glGenBuffers(1, &vertexIndexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(int), indices, GL_DYNAMIC_DRAW);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(int), indices, GL_STATIC_DRAW);

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
        //glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    }
}

void Water::render(glm::mat4 const & P, glm::mat4 const & V, glm::mat4 const & M, glm::vec3 cameraPos)
{
    prog->bind();

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));
    glUniform3f(prog->getUniform("cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
    glUniform3f(prog->getUniform("lightPos"), -2.0, 2.0, 2.0); // TODO

    // flat grey // TODO
    glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
    glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
    glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
    glUniform1f(prog->getUniform("shine"), 4.0);

    draw();

    prog->unbind();
}


void Water::initWater()
{
    prog = std::make_shared<Program>();
    prog->setVerbose(true);
    prog->setShaderNames(
            "../resources/water_vert.glsl",
            "../resources/water_frag.glsl");
    if (! prog->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");

    //prog->addUniform("Texture");
    //initTex();

    prog->addUniform("MatAmb");
    prog->addUniform("MatDif");
    prog->addUniform("MatSpec");
    prog->addUniform("shine");
    prog->addUniform("lightPos");
    prog->addUniform("cameraPos");


    prog->addAttribute("vertPos");
    //prog->addAttribute("vertNor");
}

