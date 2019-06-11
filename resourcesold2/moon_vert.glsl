#version  330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform vec3 view;


out vec3 fragNor;
out vec3 viewVec;
out vec4 pos;

void main()
{

    fragNor = normalize(vertNor.xyz);
    viewVec = normalize(view - (V * M * vec4(normalize(fragNor), 1.0)).xyz);

    gl_Position = P * V * M * vec4(vertPos.xyz, 1.0);
    pos = M * vec4(vertPos.xyz, 1.0);

}