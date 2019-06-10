#version 330 core

uniform vec3 MatDif;
uniform sampler2D bloomBuf;
uniform sampler2D sceneBuf;
in vec2 texCoord;

out vec4 Outcolor;

void main() {

    vec3 bloomCol = texture(bloomBuf, texCoord).rgb;
    vec3 sceneCol = texture(sceneBuf, texCoord).rgb;

    Outcolor = vec4(bloomCol + sceneCol, 1.0);

    //Outcolor = vec4(sceneCol, 1.0);
}