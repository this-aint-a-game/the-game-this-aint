#version 330 core

// Reference: https://learnopengl.com/Advanced-Lighting/Bloom

uniform vec3 MatDif;
uniform sampler2D bloomBuf;
uniform sampler2D sceneBuf;
in vec2 texCoord;

out vec4 Outcolor;

void main() {

    vec3 bloomCol = texture(bloomBuf, texCoord).rgb;
    vec3 sceneCol = texture(sceneBuf, texCoord).rgb;

    vec3 color = bloomCol + sceneCol;
    Outcolor = vec4(color, 1.0);
}