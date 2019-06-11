#version 330 core

uniform vec3 MatDif;
uniform sampler2D bloomBuf;
uniform sampler2D sceneBuf;
in vec2 texCoord;

out vec4 Outcolor;

void main() {

    //const float gamma = 2.2;
    //float exposure = 1.0;
    vec3 bloomCol = texture(bloomBuf, texCoord).rgb;
    vec3 sceneCol = texture(sceneBuf, texCoord).rgb;

    vec3 color = bloomCol + sceneCol;

    //vec3 result = vec3(1.0) - exp(-color * exposure);

    //result = pow(result, vec3(1.0 / gamma));

    Outcolor = vec4(color, 1.0);

    //Outcolor = vec4(sceneCol, 1.0);
}