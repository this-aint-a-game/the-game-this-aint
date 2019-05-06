#version  330 core

layout(location = 0) in vec4 vertPos;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 cameraPos;

out float water;
out vec4 pos;

void main()
{
    water = 0.0;

    gl_Position = P * V * M * vec4(vertPos.xyz, 1.0f);
    pos =  M * vec4(vertPos.xyz, 1.0f);


}
