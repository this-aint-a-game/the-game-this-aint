//
// Created by Caroline Cullen on 2019-05-04.
//

#include "Lighting.h"


Lighting::Lighting(){    numberLights = 3;  }

void Lighting::init() {
    GLubyte lightPositions[(int)numberLights][(int)numberLights][3];
//    int i, j, c;
//    for (i = 0; i < 16; i++) {
//        for (j = 0; j < 16; j++) {
//            c = (((i&0x8)==0)^((j&0x8))==0)*255;
//            lightPositions[i][j][0] = (GLubyte) c;
//            lightPositions[i][j][1] = (GLubyte) c;
//            lightPositions[i][j][2] = (GLubyte) c;
//        }
//    }

    positions.push_back(glm::vec3(50.f, 20.f, 2.f));

    lightPositions[0][0][0] = (GLubyte) 50.0;
    lightPositions[0][1][1] = (GLubyte) 20.0;
    lightPositions[0][2][2] = (GLubyte) 2.0;
    lightPositions[1][0][0] = (GLubyte) -1.0;
    lightPositions[1][1][1] = (GLubyte) 40.0;
    lightPositions[1][2][2] = (GLubyte) 2.0;
    lightPositions[2][0][0] = (GLubyte) 20.0;
    lightPositions[2][1][1] = (GLubyte) 10.0;
    lightPositions[2][2][2] = (GLubyte) 50.0;
    lightPositions[3][0][0] = (GLubyte) 20.0;
    lightPositions[3][1][1] = (GLubyte) 10.0;
    lightPositions[3][2][2] = (GLubyte) 50.0;
    lightPositions[4][0][0] = (GLubyte) 20.0;
    lightPositions[4][1][1] = (GLubyte) 10.0;
    lightPositions[4][2][2] = (GLubyte) 50.0;
    lightPositions[5][0][0] = (GLubyte) 100.0;
    lightPositions[5][1][1] = (GLubyte) 10.0;
    lightPositions[5][2][2] = (GLubyte) 100.0;



    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &lightTexture);
    glBindTexture(GL_TEXTURE_2D, lightTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, numberLights, numberLights, 0, GL_RGB, GL_UNSIGNED_BYTE, lightPositions);
}

void Lighting::bind(GLint handle)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, lightTexture);
    glUniform1i(handle, 0);
}

void Lighting::unbind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
}