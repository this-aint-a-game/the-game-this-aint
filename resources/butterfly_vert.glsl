#version  330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec2 texCoords;

void main() {

  //texCoords = vertTex;
  texCoords = (vertPos.xy+vec2(1, 1))/2.0;

  /* First model transforms */
  //gl_Position = P * V * M * vec4(vertPos.xyz, 1.0);
  gl_Position = vec4(vertPos, 1.0);


}

