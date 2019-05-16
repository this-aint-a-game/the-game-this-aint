#version  330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 LS;
uniform vec3 cameraPos;

out vec2 vTexCoord;
out vec4 fPosLS;

out vec4 pos;

void main()
{
    gl_Position = P * V * M * vec4(vertPos.xyz, 1.0f);

    pos =  M * vec4(vertPos.xyz, 1.0f);

    fPosLS = LS*pos;
}
