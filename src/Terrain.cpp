
#include "Terrain.h"

Terrain::Terrain()
{
    octave = 0;
    freq = 6.0;
    power = 1.0;
    //generateGrid();
}

void Terrain::initPermTexture(GLuint *ID)
{
    //char *pixels;
    char pixels[256 * 256 * 4];

    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, ID);
    glBindTexture(GL_TEXTURE_2D, *ID);

    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            int offset = (i * 256 + j) * 4;
            char value = perm[(j + perm[i]) & 0xFF];
            pixels[offset] = grad3[value & 0x0F][0] * 64 + 64;	 //gradient x
            pixels[offset + 1] = grad3[value & 0x0F][1] * 64 + 64; //gradient y
            pixels[offset + 2] = grad3[value & 0x0F][2] * 64 + 64; //gradient z
            pixels[offset + 3] = value;							   //permuted index
        }
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glActiveTexture(GL_TEXTURE0);
}

void Terrain::render(glm::mat4 const & P, glm::mat4 const & V, glm::mat4 const & M, glm::vec3 cameraPos)
{
    prog->bind();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, permTextureID);
    glUniform1i(prog->getUniform("permTexture"), 1);

    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P));
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, glm::value_ptr(V));
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M));
    glUniform3f(prog->getUniform("cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
    glUniform3f(prog->getUniform("lightPos"), -2.0, 2.0, 2.0); // TODO

    glUniform1i(prog->getUniform("octave"), octave);
    glUniform1f(prog->getUniform("freq"), freq);
    glUniform1f(prog->getUniform("power"), power);

    draw();

    prog->unbind();
}

void Terrain::generateGrid()
{
    //std::cout << "66" << std::endl;
    glGenVertexArrays(1, &vertexArrayID);
    //std::cout << "68" << std::endl;
    glBindVertexArray(vertexArrayID);
    //std::cout << "70" << std::endl;

    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    //std::cout << "73" << std::endl;

    float spacing = 2.f/(GRID_DIMENSION - 1.0f);

    for (int i = 0; i < GRID_DIMENSION; i++)
    {
        for (int j = 0; j < GRID_DIMENSION; j++)
        {
            vertices.push_back(-1.0f + j*spacing);
            vertices.push_back(-1.0f + i*spacing);
        }
    }

    //std::cout << "86" << std::endl;

    for (int i = 0; i < GRID_DIMENSION - 1; i++)
    {
        for (int j = 0; j < GRID_DIMENSION - 1; j++)
        {
            indices.push_back(i*GRID_DIMENSION + j);
            indices.push_back(i*GRID_DIMENSION + j + 1);
            indices.push_back((i + 1)*GRID_DIMENSION + j + 1);
            indices.push_back((i + 1)*GRID_DIMENSION + j);
        }
    }

    numIndices = indices.size();

    //std::cout << "101" << std::endl;

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_DYNAMIC_DRAW);

    glGenBuffers(1, &vertexIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    //std::cout << "114" << std::endl;
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
    glBindVertexArray(vertexArrayID);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Terrain::initTerrain()
{
    //initialize the textures we might use
    //std::cout << "127" << std::endl;
    prog = std::make_shared<Program>();
    //std::cout << "129" << std::endl;
    prog->setVerbose(true);
    //std::cout << "131" << std::endl;
    prog->setShaderNames(
            "../resources/terrain_vert.glsl",
            "../resources/terrain_frag.glsl");
    //std::cout << "135" << std::endl;
    if (! prog->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    //std::cout << "141" << std::endl;
    prog->addUniform("P");
    prog->addUniform("V");
    prog->addUniform("M");

    prog->addUniform("permTexture");

    prog->addUniform("MatAmb");
    prog->addUniform("MatDif");
    prog->addUniform("MatSpec");
    prog->addUniform("shine");
    prog->addUniform("lightPos");
    prog->addUniform("cameraPos");

    // TODO set key callbacks
    prog->addUniform("mode");
    prog->addUniform("freq");
    prog->addUniform("octave");
    prog->addUniform("power");

    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");

    // TODO
    //Initialize textures
    initPermTexture(&permTextureID);
    //initSimplexTexture(&simplexTextureID);
    //initGradTexture(&gradTextureID);
}

void Terrain::updateOctave()
{
    if (octave == true)
    {
        octave = false;
    }
    else
    {
        octave = true;
    }
}
void Terrain::updateFreq(bool increase)
{
    if (increase)
    {
        freq += 0.25;
    }
    else
    {
        freq -= 0.25;
    }
}

void Terrain::updatePower(bool increase)
{
    if (increase)
    {
        power += 0.05;
    }
    else
    {
        power -= 0.05;
    }
}


