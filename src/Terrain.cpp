
#include "Terrain.h"

Terrain::Terrain()
{
    octave = 0;
    freq = 1.0;
    power = 0.5;
}

void Terrain::initPermTexture(GLuint *ID)
{
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
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, glm::value_ptr(M));
    glUniform3f(prog->getUniform("cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
    glUniform3f(prog->getUniform("lightPos"), -2.0, 2.0, 2.0); // TODO

    glUniform1i(prog->getUniform("octave"), octave);
    glUniform1f(prog->getUniform("freq"), freq);
    glUniform1f(prog->getUniform("power"), power);

    // flat grey
    glUniform3f(prog->getUniform("MatAmb"), 0.13, 0.13, 0.14);
    glUniform3f(prog->getUniform("MatDif"), 0.3, 0.3, 0.4);
    glUniform3f(prog->getUniform("MatSpec"), 0.3, 0.3, 0.4);
    glUniform1f(prog->getUniform("shine"), 4.0);

    // pearl
    /*
    glUniform3f(prog->getUniform("MatAmb"), 0.25f, 0.20725f, 0.20725f);
    glUniform3f(prog->getUniform("MatDif"), 1.0f, 0.829f, 0.829f);
    glUniform3f(prog->getUniform("MatSpec"), 0.296648f, 0.296648f, 0.296648f);
    glUniform1f(prog->getUniform("shine"), 128.0f * 0.088f);
    */

    draw();

    prog->unbind();
}


void Terrain::generateGrid()
{

    int VERTEX_COUNT = 300;
    int SIZE = 25;

    GLfloat vertices[300 * 300 * 3];
    GLint indices[6 * (299) * (299)];

    int vertexPointer = 0;
    for (int i = 0; i < VERTEX_COUNT; i++)
    {
        for (int j = 0; j < VERTEX_COUNT; j++)
        {
            vertices[vertexPointer * 3] = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
            vertices[vertexPointer * 3 + 1] = 0;
            vertices[vertexPointer * 3 + 2] = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;
            vertexPointer++;
        }
    }

    int pointer = 0;
    for (int gz = 0; gz < VERTEX_COUNT - 1; gz++)
    {
        for (int gx = 0; gx < VERTEX_COUNT - 1; gx++)
        {
            int topLeft = (gz * VERTEX_COUNT) + gx;
            int topRight = topLeft + 1;
            int bottomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
            int bottomRight = bottomLeft + 1;

            indices[pointer++] = topLeft;
            indices[pointer++] = bottomLeft;
            indices[pointer++] = topRight;
            indices[pointer++] = topRight;
            indices[pointer++] = bottomLeft;
            indices[pointer++] = bottomRight;
        }
    }

    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

    glGenBuffers(1, &vertexIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
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
    glDrawElements(GL_TRIANGLES, 300 * 300 * 10, GL_UNSIGNED_INT, nullptr);
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

    prog->addUniform("permTexture");

    prog->addUniform("MatAmb");
    prog->addUniform("MatDif");
    prog->addUniform("MatSpec");
    prog->addUniform("shine");
    prog->addUniform("lightPos");
    prog->addUniform("cameraPos");

    prog->addUniform("mode");
    prog->addUniform("freq");
    prog->addUniform("octave");
    prog->addUniform("power");

    prog->addAttribute("vertPos");
    prog->addAttribute("vertNor");

    //Initialize textures
    initPermTexture(&permTextureID);
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


