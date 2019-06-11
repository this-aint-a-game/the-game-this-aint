#version 330 core

uniform sampler2D sceneBuf;
in vec2 texCoord;

out vec4 BrightColor;

void main() {

    vec3 sceneCol = texture(sceneBuf, texCoord).rgb;

    float brightness = dot(sceneCol, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.5)
    {
        BrightColor = vec4(sceneCol.rgb, 1.0);
    }
    else
    {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }

    //BrightColor = vec4(brightness, 0, 0, 1.0);
    //BrightColor = vec4(sceneCol.rgb, 1.0);
}