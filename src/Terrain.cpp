
#include "Terrain.h"
#include "obtain_noise.h"

Terrain::Terrain(ColorCollectGameplay * ccg)
{
    this->ccg = ccg;
    vertexArrayID = 0;
    generateGrid(INITIAL_WIDTH);

    std::thread t = threadedGenerateGrid();
    t.detach();
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

void Terrain::render(glm::mat4 const & P, glm::mat4 const & V, glm::mat4 const & M, glm::mat4 & LS, GLuint depthMap, glm::vec3 cameraPos, Lighting* lighting, glm::vec3 lightPos) {
    prog->bind();

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));
    glUniform3f(prog->getUniform("cameraPos"), cameraPos.x, cameraPos.y,
                cameraPos.z);
    glUniform3f(prog->getUniform("lightPos"), lightPos.x, lightPos.y,
                lightPos.z);
    glUniform1f(prog->getUniform("numberLights"), lighting->numberLights);
    lighting->bind(prog->getUniform("lighting"));

    if (ccg->checkColor(5) && ccg->checkColor(1)) {

        glm::vec3 d = ccg->violetColor.diffuse;
        glm::vec3 d_o  = ccg->orangeColor.diffuse;
        glUniform3f(prog->getUniform("MatAmb"), 0.f, 0.f, 0.f);
        glUniform3f(prog->getUniform("MatDif"), d_o.x, d_o.y, d_o.z);
        glUniform3f(prog->getUniform("MatSpec"), d.x, d.y, d.z);
        glUniform1f(prog->getUniform("shine"), 50.f);
    }
    else if (ccg->checkColor(5)) {

        glm::vec3 d = ccg->violetColor.diffuse;
        glUniform3f(prog->getUniform("MatAmb"), 0.f, 0.f, 0.f);
        glUniform3f(prog->getUniform("MatDif"), 0.5f, 0.5f, 0.5f);
        glUniform3f(prog->getUniform("MatSpec"), d.x, d.y, d.z);
        glUniform1f(prog->getUniform("shine"), 50.f);
    }
    else if (ccg->checkColor(1)) {

        glm::vec3 d_o  = ccg->orangeColor.diffuse;
        glm::vec3 d_a  = ccg->orangeColor.ambient;
        glUniform3f(prog->getUniform("MatDif"), d_o.x, d_o.y, d_o.z);
        glUniform3f(prog->getUniform("MatSpec"), 0, 0, 0);
        glUniform1f(prog->getUniform("shine"), 50.f);
    }
    else
    {
        glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.13);
        glUniform3f(prog->getUniform("MatDif"), 0.5f, 0.5f, 0.5f);
        glUniform3f(prog->getUniform("MatSpec"), 0, 0, 0);

    }

    // TODO

    //glActiveTexture(GL_TEXTURE3); // TODO? 2?
    //glBindTexture(GL_TEXTURE_2D, depthMap);

    //glUniform1i(prog->getUniform("shadowDepth"), 3); // TODO 1?
    //glUniformMatrix4fv(prog->getUniform("LS"), 1, GL_FALSE, value_ptr(LS));

    draw();

    //terrainTexture->unbind();
    prog->unbind();
}

void Terrain::computeIndicesForClipVolume(int width, float clipx0, float clipx1, float clipz0, float clipz1)
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
    //std::cout << "numIndices: " << numIndices << std::endl;
}

void Terrain::bindVAO()
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

        /*
        static GLfloat GrndTex[] = {
                0, 0, // back
                0, 1,
                1, 1,
                1, 0 };
        glGenBuffers(1, &GrndTexBuffObj);
        glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GrndTex), GrndTex, GL_STATIC_DRAW);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, GrndTexBuffObj);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
         */


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

void Terrain::generateGrid(int width)
{
    int start = -width/2;//0;
    int finish = width/2;//0;

    std::vector <glm::vec3> tempV;

    for (int vx = start; vx < finish; vx++)
    {
        for (int vz = start; vz < finish; vz++)
        {
            float x = vx * STEP;
            float z = -vz * STEP;
            float y = calcHeight(x, z);

            glm::vec3 v0 = glm::vec3(x, y, z);

            // TODO calculate normal

            tempV.push_back(v0);
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
    vertices = tempV;
    vertexArrayID = 0;


}

std::thread Terrain::threadedGenerateGrid()
{
    return std::thread([this] { this->generateGrid(); });
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
    bindVAO();
    glDrawElements(GL_TRIANGLE_STRIP, numIndices, GL_UNSIGNED_INT, 0);
    unbind();
}

void Terrain::unbind()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    //glActiveTexture(GL_TEXTURE0);

    //glDisableVertexAttribArray(2);
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

    //prog->addUniform("Texture");
    //initTex();

    prog->addUniform("MatAmb");
    prog->addUniform("MatDif");
    prog->addUniform("MatSpec");
    prog->addUniform("shine");
    prog->addUniform("lighting");
    prog->addUniform("cameraPos");
    prog->addUniform("numberLights");
    prog->addUniform("shadowDepth");
    prog->addUniform("LS");
    prog->addUniform("lightPos");

    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");
    //prog->addAttribute("vertTex");
}
